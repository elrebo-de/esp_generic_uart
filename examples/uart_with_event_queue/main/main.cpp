/*
 * Example program to use a GNSS receiver (LC76G) with elrebo-de/generic_uart
 */

#include <string>
#include <vector>
#include <sstream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "generic_uart.hpp"
#include "esp_log.h"

std::string tag = "Basic LC76G GNSS receiver";

void readCb(std::string stringBuffer)
{
    ESP_LOGI(tag.c_str(), "Buffer(length=%i): \n%s", stringBuffer.length(), stringBuffer.c_str());
}

extern "C" void rx_task(void *arg)
{
    GenericUart* uart = (GenericUart *) arg;
    uart_event_t event;

    while (1) {
        // Wait for UART events with timeout to allow task yielding
        if (xQueueReceive(uart->getUartQueue(), (void *)&event, pdMS_TO_TICKS(100))) {
            switch (event.type) {
                case UART_DATA:
                {
                    if(event.timeout_flag) {
                        // call readCb with received data
                        readCb(uart->readString(100));
                    }
                    break;
                }

                case UART_FIFO_OVF:
                    ESP_LOGW(uart->getTag().c_str(), "UART FIFO overflow");
                    uart_flush_input(uart->getUartNum());
                    xQueueReset(uart->getUartQueue());
                    break;

                case UART_BUFFER_FULL:
                    ESP_LOGW(uart->getTag().c_str(), "UART ring buffer full");
                    uart_flush_input(uart->getUartNum());
                    xQueueReset(uart->getUartQueue());
                    break;

                default:
                    break;
            }
        }
        // Yield to other tasks periodically
        taskYIELD();
    }
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    vTaskDelay(500 / portTICK_PERIOD_MS); // delay 0.5 seconds

    ESP_LOGI(tag.c_str(), "Basic LC76G GNSS receiver Example Program using GenericUart with rx_task and event queue");

    // set UART configuration
    const uart_port_t uart_num = UART_NUM_2;
    uint32_t uart_buffer_size = 2048;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
#if ESP_IDF_VERSION_MAJOR == 6
        .rx_glitch_filt_thresh = 0,
#endif
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {
           .allow_pd = 0,
           .backup_before_sleep = 0,
        },
    };
    // M5STACK CORE2 V1.1
    const int uart_tx_pin = 32;
    const int uart_rx_pin = 33;

    // M5ATOM LITE
    // const int uart_tx_pin = 26;
    // const int uart_rx_pin = 32;

    {
        ESP_LOGI(tag.c_str(), "GenericUart with rx_task");
        GenericUart gnssUart(
                     "LC76G GNSS UART",
                     uart_num,
                     &uart_config,
                     uart_tx_pin,
                     uart_rx_pin,
                     uart_buffer_size,
                     rx_task);

        std::string command;
        esp_err_t ret;
        // send PAIR command cold start
        command = "$PAIR006*3C\r\n";
        ret = gnssUart.writeString(command);
        ESP_LOGI(tag.c_str(), "Pair-Command: %s ret: %s", command.c_str(), esp_err_to_name(ret));
        vTaskDelay(10 / portTICK_PERIOD_MS); // delay 10 ms

        // send PAIR command to set 1Hz update rate
        command = "$PAIR050,1000*12\r\n";
        ret = gnssUart.writeString(command);
        ESP_LOGI(tag.c_str(), "Pair-Command: %s ret: %s", command.c_str(), esp_err_to_name(ret));
        vTaskDelay(10 / portTICK_PERIOD_MS); // delay 10 ms

        // do nothing
        while(1) {
            vTaskDelay(30000 / portTICK_PERIOD_MS); // delay 30 seconds
        }
    }
}
