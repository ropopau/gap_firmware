/*
 * uart.c
 *
 *  Created on: May 14, 2025
 *      Author: sanghyeon
 */

#include "utils/uart.h"

static UartDevice uart_devices[] = {
    { .handler = 0, .state = UART_STATE_ENABLED, .name = "USART2" },
    { .handler = 0, .state = UART_STATE_ENABLED, .name = "USART6" },
};


void uart_set_handler(UART_HandleTypeDef *debug_uart_handler, UART_HandleTypeDef *control_uart_handler) {
	uart_devices[DEBUG_UART].handler = debug_uart_handler;
	uart_devices[CONTROL_UART].handler = control_uart_handler;

}


void uart_enable(enum UartTypes huart_type) {
	UartDevice uart_infos = uart_devices[huart_type];


	if (uart_infos.state == UART_STATE_ENABLED) {
		send_log(VERBOSITY_WARNING, "%s is already enabled", uart_infos.name);
		return;
	}
	send_log(VERBOSITY_INFO, "Enabling %s ", uart_infos.name);

	HAL_UART_Init(uart_infos.handler);
	uart_infos.state = UART_STATE_ENABLED;

}

void uart_disable(enum UartTypes huart_type) {
	UartDevice uart_infos = uart_devices[huart_type];

	if (uart_infos.state  == UART_STATE_DISABLED) {
		send_log(VERBOSITY_WARNING, "%s is already disabled", uart_infos.name);
		return;
	}
	send_log(VERBOSITY_INFO, "Disabling %s ", uart_infos.name);


	HAL_UART_DeInit(uart_infos.handler);
	uart_infos.state = UART_STATE_DISABLED;
}



