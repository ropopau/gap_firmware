/**
  ******************************************************************************
  * @file    uart.h
  * @author  Group 13
  * @brief   Header for uart.c file.
  ******************************************************************************
  */

#ifndef SRC_UART_H
#define SRC_UART_H

#include <stdint.h>
#include "log_manager.h"
#include "main.h"

/**
 * @brief	Define State for a given UART.
 */
enum UartState {
    UART_STATE_DISABLED,
    UART_STATE_ENABLED
};

/**
 * @brief	Define UART by a more explicit name.
 *
 * If adding more uart, you should also take account of the array `uart_devices` in uart.c
 * so the enum value match the array element index.
 */
enum UartTypes {
	DEBUG_UART,
	CONTROL_UART
};

/**
 *  @brief	Define a structure for an uart handler with its current State and name
 *  (the name is mostly for logging purpose)
 */
typedef struct {
	UART_HandleTypeDef *handler;
    enum UartState state;
    const char *name;

} UartDevice;


/* Exported functions prototypes ---------------------------------------------*/
void uart_set_handler(UART_HandleTypeDef *debug_uart_handler, UART_HandleTypeDef *control_uart_handler);
void uart_enable(enum UartTypes huart_type);
void uart_disable(enum UartTypes huart_type);

#endif // SRC_UART_H
