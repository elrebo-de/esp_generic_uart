# GenericUart component

This repository contains an ESP-IDF component for a UART. It runs on
any ESP32 processor and is built using the ESP-IDF build system in version 5.5.0+.

The component is implemented as C++ class `GenericUart`.

## Connecting the component and Usage

You need to include ```generic_uart.hpp```.

In your program you have to set the configuration for the UART and create an instance of clas `GenericUart`.

```C++
extern "C" void app_main(void)
{
    vTaskDelay(500 / portTICK_PERIOD_MS); // delay 0.5 seconds

    ESP_LOGI(tag, "LC76G GNSS receiver Example Program");

    // set UART configuration
    const uart_port_t uart_num = UART_NUM_2;

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

    GenericUart gnssUart(
                 "LC76G GNSS UART",
                 uart_num,
                 &uart_config,
                 uart_tx_pin,
                 uart_rx_pin);

    // this method reads up to 2047 bytes from the UART with a timeout of 100 ms
    // if nothing is read, the length of stringRead is 0
    std::string stringRead = gnssUart.readString(100);
...
```

Currently class `GenericUart` only supports reading data with method `readString`. The string size is limited to 2047 bytes.

((tbc))

# License

This component is provided under the Apache 2.0 license.
