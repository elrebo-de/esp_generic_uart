# GenericUart component

This repository contains an ESP-IDF component for UART communications. It runs on
any ESP32 processor and is built using the ESP-IDF build system in version 5.5.0+.

The component is implemented as C++ class `GenericUart`.

## Connecting the component and Usage

You need to include ```generic_uart.hpp```.

In your program you have to set the configuration for the UART and create an instance of class `GenericUart`.

This snippet shows how to use the GenericUart class to read the NMEA records from a GNSS positioning system. 

It uses method `writeString()` to set the update rate of the GNSS receiver and method `readString()` to read the NMEA records from the GNSS receiver.

See example uart_without_event_queue for details.

```C++
extern "C" void app_main(void)
{
    vTaskDelay(500 / portTICK_PERIOD_MS); // delay 0.5 seconds

    ESP_LOGI(tag.c_str(), "Basic LC76G GNSS receiver Example Program");

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
        ESP_LOGI(tag.c_str(), "first test: GenericUart without rx_task");

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

        // check for data with a timeout of 10 ms 100 times
        for(int i =0; i < 100; i++) {
            std::string data = gnssUart.readString(100); // read received data
            ESP_LOGI(tag.c_str(), "Received Data (%i): \n%s", i, data.c_str());
        }
    }
...
```

In the example method `readString` is called repeatedly to read the NMEA data.
If no data is available after the rx_timeout of 100 ms, the function returns an empty string.

There is another possibility to use `GenericUart`. You can listen to the UART event queue. This approach is shown in example uart_with_event_queue.

You have to supply a function to be run in a separate task, which listens to the UART event queue and triggers the read operation when the availability of data is announced by an event.

This function is given to class `GenericUart` in the constructor and the task is created inside the constructor.

In the example uart_with_event_queue the function `rx_task` triggers a `readCb` callback function, when the NMEA data has been received by the `GenericUart` class.

Code for `readCb`and `rx_task` functions:

```C++
void readCb(char* buffer, int length)
{
    buffer[length] = 0x00;
    ESP_LOGI(tag.c_str(), "Buffer(length=%i): \n%s", length, buffer);
}

extern "C" void rx_task(void *arg)
{
    GenericUart* uart = (GenericUart *) arg;
    uart_event_t event;
    uint8_t *buffer = (uint8_t *)malloc(uart->getUartBufferSize());
    //uint32_t output_buffer_len = 0;
    //char *output_buffer = (char *) malloc(uart->getUartBufferSize());

    if (!buffer  /*|| !output_buffer*/) {
        ESP_LOGE(uart->getTag().c_str(), "Failed to allocate RX buffers");
        if (buffer) free(buffer);
        //if (output_buffer) free(output_buffer);
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        // Wait for UART events with timeout to allow task yielding
        if (xQueueReceive(uart->getUartQueue(), (void *)&event, pdMS_TO_TICKS(100))) {
            switch (event.type) {
                case UART_DATA:
                {
                    if(event.timeout_flag) {
                        int len = uart_read_bytes(uart->getUartNum(), buffer, uart->getUartBufferSize() - 1, pdMS_TO_TICKS(100));
                        readCb((char *)buffer, len);
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

    free(buffer);
    //free(output_buffer);
    vTaskDelete(NULL);
}
```
Code snippet from app_main:

```C++
    {
        ESP_LOGI(tag.c_str(), "second test: GenericUart with rx_task");
        GenericUart gnssUart(
                     "LC76G GNSS UART",
                     uart_num,
                     &uart_config,
                     uart_tx_pin,
                     uart_rx_pin,
                     uart_buffer_size,
                     rx_task);

        // do nothing
        while(1) {
            vTaskDelay(30000 / portTICK_PERIOD_MS); // delay 30 seconds
        }
    }
```
The component is tested on ESP-IDF versions 5.5.5 and 6.1.

# License

This component is provided under the Apache 2.0 license.
