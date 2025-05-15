/**
  ******************************************************************************
  * @file    uart.c
  * @author  Group 13
  * @brief   This file manage every uart used by the system. It can enable or disable an UART.
  *
  * - A static array `uart_devices` is defined to list every used uart.
  *
  ******************************************************************************
  */

#include "utils/uart.h"



// Array that list every uart set and used by this system.
static UartDevice uart_devices[] = {
    { .handler = 0, .state = UART_STATE_ENABLED, .name = "USART2" },
    { .handler = 0, .state = UART_STATE_ENABLED, .name = "USART6" },
};

/**
  * @brief  Set known uart's handler into the uart_devices static array.
  * 		You must add into this function argument the uarts you would add in the future.
  * 		Please look at the description of the enum's defined in uart.h to learn more.
  *
  * @param  UART_HandleTypeDef *debug_uart_handler	handler of the debug uart (uart2)
  * @param 	UART_HandleTypeDef *control_uart_handler  handler of the constrol uart (uart6)
  * @retval None
  */
void uart_set_handler(UART_HandleTypeDef *debug_uart_handler, UART_HandleTypeDef *control_uart_handler) {
	uart_devices[DEBUG_UART].handler = debug_uart_handler;
	uart_devices[CONTROL_UART].handler = control_uart_handler;

}


/**
  * @brief  Enable an uart
  *
  * @param  enum UartTypes huart_type	uart to enable
  * @retval None
  */
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

/**
  * @brief  Disable an uart
  *
  * @param  enum UartTypes huart_type	uart to disable
  * @retval None
  */
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



