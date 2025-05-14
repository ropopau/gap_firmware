/*
 * log_manager.c
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#include "log_manager.h"


UART_HandleTypeDef *huart_handler;

static char buf[1024];
static enum VERBOSITY_LEVEL current_verbosity = VERBOSITY_INFO;
static const char* verbosity_strings[] = { "Debug", "Info", "Warning", "Error" };



void init_log(UART_HandleTypeDef *huart)
{
	huart_handler = huart;
}


void disable_debug_uart() {
	HAL_NVIC_DisableIRQ(USART2_IRQn);
	//HAL_UART_abort(huart_handler);

	HAL_UART_DeInit(huart_handler);

	__HAL_RCC_USART2_CLK_DISABLE();
}

void enable_debug_uart() {
	__HAL_RCC_USART2_CLK_ENABLE();

	huart_handler->Instance = USART2;
	huart_handler->Init.BaudRate = 115200;
	huart_handler->Init.WordLength = UART_WORDLENGTH_8B;
	huart_handler->Init.StopBits = UART_STOPBITS_1;
	huart_handler->Init.Parity = UART_PARITY_NONE;
	huart_handler->Init.Mode = UART_MODE_TX;
	huart_handler->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart_handler->Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_NVIC_EnableIRQ(USART2_IRQn);

	if (HAL_UART_Init(huart_handler) != HAL_OK)
	{
		Error_Handler();
	}
}





void change_verbosity(enum VERBOSITY_LEVEL verbosity) {

	if (current_verbosity == verbosity){
		send_log(VERBOSITY_WARNING, "Current log verbosity is already %s. Request ignored", verbosity_strings[current_verbosity]);
	}
	else{
		current_verbosity = verbosity;

	}

}


void send_log(enum VERBOSITY_LEVEL verbosity, const char *message, ...)
{
	// current -> Info, error -> alors il fait rien
    if (current_verbosity > verbosity)
        return;

    const char *level = verbosity_strings[verbosity];
    char formatted[512];

    va_list args;
    va_start(args, message);

    vsnprintf(formatted, sizeof(formatted), message, args);
    va_end(args);
    sprintf(buf, "[%s] %s\r\n", level, formatted);

    HAL_UART_Transmit(huart_handler, (uint8_t*)buf, (uint16_t )strlen(buf), HAL_MAX_DELAY);
}
