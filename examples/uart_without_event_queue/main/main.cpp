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
#include "bsp/esp-bsp.h"
#include "bsp/m5stack_core_2.h"

std::string tag = "Basic LC76G GNSS receiver";

extern "C" void app_main(void)
{
    vTaskDelay(500 / portTICK_PERIOD_MS); // delay 0.5 seconds

    ESP_LOGI(tag.c_str(), "Basic LC76G GNSS receiver Example Program using GenericUart without rx_task and event queue");

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

    bsp_i2c_init();
    bsp_display_start();

    // M5ATOM LITE
    // const int uart_tx_pin = 26;
    // const int uart_rx_pin = 32;

    {
        ESP_LOGI(tag.c_str(), "GenericUart without rx_task");

        GenericUart gnssUart(
                     "LC76G GNSS UART",
                     uart_num,
                     &uart_config,
                     uart_tx_pin,
                     uart_rx_pin,
                     uart_buffer_size,
                     NULL);

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

        // check for data with a timeout of 500 ms for 5 minutes
        for(int i =0; i < 600; i++) {
            std::string data = gnssUart.readString(500); // read received data
            ESP_LOGI(tag.c_str(), "Received Data (%i): \n%s", i, data.c_str());
        }
    }
}
