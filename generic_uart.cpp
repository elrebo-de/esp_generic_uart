/*
 * generic_uart.cpp
 *
 *  Created on: 11.08.2026
 *      Author: christophoberle
 */

#include "generic_uart.hpp"
#include "esp_log.h"

GenericUart::GenericUart( std::string tag,
                          uart_port_t uart_num,
                          uart_config_t *uart_config,
                          int uart_tx_pin,
                          int uart_rx_pin) {
	this->tag = tag;
    this->uart_num = uart_num;
    this->uart_config = uart_config;
    this->uart_tx_pin = uart_tx_pin;
    this->uart_rx_pin = uart_rx_pin;

    ESP_LOGI(this->tag.c_str(), "constructor");

    ESP_LOGI(this->tag.c_str(), "UART_HW_FIFO_LEN(%i): %i", uart_num, UART_HW_FIFO_LEN(uart_num));

    // Install UART driver using no event queue
    ESP_ERROR_CHECK(uart_driver_install(this->uart_num, this->uart_buffer_size, this->uart_buffer_size, 10, NULL /* war &this->uart_queue */, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(this->uart_num, this->uart_config));

    // Set UART pins(TX, RX, RTS, CTS, DTR, DSR)
    ESP_ERROR_CHECK(uart_set_pin(this->uart_num, this->uart_tx_pin, this->uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

GenericUart::~GenericUart() {
	// TODO Auto-generated destructor stub
}

uart_port_t GenericUart::getUartNum() {
	return this->uart_num;
}

std::string GenericUart::readString(int timeoutMs) {
    int len = uart_read_bytes(this->uart_num, this->data, 2048 - 1, timeoutMs / portTICK_PERIOD_MS);

    data[len] = '\0';
    std::string bytesRead((char *)data);

	return bytesRead;
}

