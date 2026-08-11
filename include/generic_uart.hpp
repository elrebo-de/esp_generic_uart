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
	GenericUart(std::string tag, uart_port_t uart_num, uart_config_t *uart_config, int uart_tx_pin, int uart_rx_pin);
	virtual ~GenericUart();

private:
    std::string tag;
    uart_port_t uart_num;
    int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    uart_config_t *uart_config;
    int uart_tx_pin;
    int uart_rx_pin;
};

#endif /* GENERIC_UART_HPP_ */
