/*
 * generic_uart.hpp
 *
 *  Created on: 11.08.2026
 *      Author: christophoberle
 */

#ifndef GENERIC_UART_HPP_
#define GENERIC_UART_HPP_

#include <stdio.h>

//#include <cstdint>
#include <string>

#include "driver/uart.h"
#include "soc/clk_tree_defs.h"
#include "hal/uart_types.h"

class GenericUart {
public:
	GenericUart(
	    std::string tag,
	    uart_port_t uart_num,
	    uart_config_t *uart_config,
	    int uart_tx_pin,
	    int uart_rx_pin,
        uint32_t uart_buffer_size,
        TaskFunction_t rx_task
	);
	virtual ~GenericUart();
	uart_port_t getUartNum();
	QueueHandle_t getUartQueue();
	uint32_t getUartBufferSize();
	std::string getTag();

	std::string readString(int timeoutMs);
    esp_err_t writeString(std::string command);
private:
    std::string tag;
    uart_port_t uart_num;
    uint32_t uart_buffer_size;
    QueueHandle_t uart_queue;
    uart_config_t *uart_config;
    int uart_tx_pin;
    int uart_rx_pin;
    TaskFunction_t rx_task;

    TaskHandle_t rx_task_handle;

    bool uart_initialized;

    uint8_t* data;
};

#endif /* GENERIC_UART_HPP_ */
