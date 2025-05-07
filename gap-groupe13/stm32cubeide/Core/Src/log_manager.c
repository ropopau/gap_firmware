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



static char buf[1024];
static enum VERBOSITY_LEVEL current_verbosity = VERBOSITY_INFO;



void change_verbosity(enum VERBOSITY_LEVEL verbosity) {

	if (current_verbosity == verbosity){
		send_log(VERBOSITY_WARNING, "Current log verbosity is already %s. Request ignored", verbosity_strings[current_verbosity]);
	}
	else{
		send_log(
						VERBOSITY_INFO,
						"Verbosity has changed from %s to %s",
						verbosity_strings[current_verbosity],
						verbosity_strings[verbosity]
						);
		current_verbosity = verbosity;

	}




}


void send_log(enum VERBOSITY_LEVEL verbosity, const char *message, ...)
{
	// current -> Info, error -> alors il fait rien
    if (current_verbosity > verbosity)
        return;

    char *level = verbosity_strings[verbosity];
    char formatted[1024];

    va_list args;
    va_start(args, message);

    vsnprintf(formatted, sizeof(formatted), message, args);

    va_end(args);

    sprintf(buf, "[%s] %s\r\n", level, formatted);

    HAL_UART_Transmit(huart_handler, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}
