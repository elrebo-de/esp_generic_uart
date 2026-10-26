/*
 * generic_uart.cpp
 *
 *  Created on: 11.08.2026
 *      Author: christophoberle
 */

#include "generic_uart.hpp"
#include "esp_log.h"

#include "esp_idf_version.h"
#include "driver/gpio.h"

GenericUart::GenericUart( std::string tag,
                          uart_port_t uart_num,
                          uart_config_t *uart_config,
                          int uart_tx_pin,
                          int uart_rx_pin,
                          uint32_t uart_buffer_size,
                          TaskFunction_t rx_task
                        ) {
    this->uart_initialized = false;

	this->tag = tag;
    this->uart_num = uart_num;
    this->uart_config = uart_config;
    this->uart_tx_pin = uart_tx_pin;
    this->uart_rx_pin = uart_rx_pin;
    this->uart_buffer_size = uart_buffer_size;
    this->rx_task = rx_task;

    this->rx_task_handle = NULL;
    this->data = NULL;

    ESP_LOGI(this->tag.c_str(), "constructor");

    //ESP_LOGI(this->tag.c_str(), "UART_HW_FIFO_LEN(%i): %i", uart_num, UART_HW_FIFO_LEN(uart_num));

    esp_err_t ret;

    // Install UART driver
    if(this->rx_task) {
        // if an rx_task function is defined, then install driver with an event queue
        ret = uart_driver_install(this->uart_num, this->uart_buffer_size * 2, this->uart_buffer_size * 2, 20, &this->uart_queue, 0);
    }
    else {
        // if no rx_task function is defined, then install driver without an event queue
        ret = uart_driver_install(this->uart_num, this->uart_buffer_size * 2, this->uart_buffer_size * 2, 0, NULL, 0);
    }

    if (ret != ESP_OK) {
        ESP_LOGE(tag.c_str(), "Failed to install UART driver");
        return;
    }

    // Configure UART parameters
    ret = uart_param_config(this->uart_num, this->uart_config);
    if (ret != ESP_OK) {
        ESP_LOGE(tag.c_str(), "Failed to configure UART parameters");
        uart_driver_delete(this->uart_num);
        return;
    }

    // Set UART pins(TX, RX, RTS, CTS, DTR, DSR)
    #if ESP_IDF_VERSION_MAJOR == 6
        ret = uart_set_pin(this->uart_num, this->uart_tx_pin, this->uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    #elif ESP_IDF_VERSION_MAJOR < 6
        ret = uart_set_pin(this->uart_num, this->uart_tx_pin, this->uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    #endif
    if (ret != ESP_OK) {
        ESP_LOGE(tag.c_str(), "Failed to set UART pins");
        uart_driver_delete(this->uart_num);
        return;
    }

    // configure rx_timeout, so that UART_DATA event is triggered after every byte stream,
    // even when FIFO buffer is full
    uart_set_always_rx_timeout(this->uart_num, true);

    // Create RX task, if function given
    if(rx_task != NULL) {
        ESP_LOGI(tag.c_str(), "Starting RX task...");
        xTaskCreate(rx_task, "uart_rx_task", 4096, (void *) this, 10, &this->rx_task_handle);
    }

    this->uart_initialized = true;
}

GenericUart::~GenericUart() {
    if (!this->uart_initialized) {
        return;
    }

    // Delete RX task
    if (rx_task_handle) {
        vTaskDelete(rx_task_handle);
        rx_task_handle = NULL;
    }

    // Delete UART driver
    uart_driver_delete(this->uart_num);

    if(this->data) {
        free(this->data);
    }
    // fix for "tx_pin is not usable, maybe used by others" when constructor is called after destructor for the second time
    gpio_reset_pin((gpio_num_t)this->uart_tx_pin);

    this->uart_initialized = false;

    ESP_LOGI(tag.c_str(), "deleted");
    return;
}

uart_port_t GenericUart::getUartNum() {
	return this->uart_num;
}

QueueHandle_t GenericUart::getUartQueue() {
	return this->uart_queue;
}

uint32_t GenericUart::getUartBufferSize() {
	return this->uart_buffer_size;
}

std::string GenericUart::getTag() {
	return this->tag;
}

std::string GenericUart::readString(int timeoutMs) {
    // lazy initialization of data buffer
    if(!this->data) {
        this->data = (uint8_t *)malloc(this->uart_buffer_size);
        if(!this->data) {
            ESP_LOGE(this->tag.c_str(), "Failed to allocate RX buffer");
            return std::string("");
        }
    }

    int len = uart_read_bytes(this->uart_num, this->data, this->uart_buffer_size - 1, timeoutMs / portTICK_PERIOD_MS);

    this->data[len] = '\0';
    return std::string((char *)this->data);
}

esp_err_t GenericUart::writeString(std::string command) {
    if (!this->uart_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (command.length() == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    // Send command
    int len = uart_write_bytes(this->uart_num, command.c_str(), command.length());
    if (len < 0) {
        ESP_LOGE(tag.c_str(), "Failed to write command: %s", command.c_str());
        return ESP_FAIL;
    }

    ESP_LOGI(tag.c_str(), "Command written: %s", command.c_str());
    return ESP_OK;
}


