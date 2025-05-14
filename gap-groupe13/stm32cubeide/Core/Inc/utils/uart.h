/*
 * uart.h
 *
 *  Created on: May 14, 2025
 *      Author: sanghyeon
 */

#ifndef SRC_UART_H
#define SRC_UART_H

#include <stdint.h>
#include "log_manager.h"
#include "main.h"

enum UartState {
    UART_STATE_DISABLED,
    UART_STATE_ENABLED
};

enum UartTypes {
	DEBUG_UART,
	CONTROL_UART
};


typedef struct {
	UART_HandleTypeDef *handler;
    enum UartState state;
    const char *name;

} UartDevice;






void uart_set_handler(UART_HandleTypeDef *debug_uart_handler, UART_HandleTypeDef *control_uart_handler);

void uart_enable(enum UartTypes huart_type);

void uart_disable(enum UartTypes huart_type);



#endif // SRC_UART_H
