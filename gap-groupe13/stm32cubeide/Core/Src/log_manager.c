/*
 * log_manager.c
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#include "log_manager.h"


UART_HandleTypeDef *huart_handler;

//static char buf[1024];

void init_log(UART_HandleTypeDef *huart)
{
	huart_handler = huart;
}

static char buf[1024];



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

	sprintf(buf, "%s %s\r\n", level, message);
	HAL_UART_Transmit(huart_handler, (uint8_t*)buf, strlen(level) + strlen(message) + 3, HAL_MAX_DELAY);
}
