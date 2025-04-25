/*
 * log_manager.c
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#include "log_manager.h"


UART_HandleTypeDef *huart_handler;

void init_log(UART_HandleTypeDef *huart)
{
	huart_handler = huart;
}

void send_log(enum VERBOSITY_LEVEL verbosity, const char *message)
{
	char *level = "[Info]";
	if (verbosity == VERBOSITY_DEBUG)
	{
		level = "[Debug]";
	}
	else if (verbosity == VERBOSITY_WARNING)
	{
		level = "[Warning]";
	}
	else if (verbosity == VERBOSITY_ERROR)
	{
		level = "[Error]";
	}
	char buf[1024];
	sprintf(buf, "%s %s\r\n", level, message);
	HAL_UART_Transmit_IT(huart_handler, (uint8_t*)buf, strlen(buf));
}
