/**
  ******************************************************************************
  * @file    log_manager.c
  * @author  Group 13
  * @brief   Managing logging through the debug uart.
  *
  ******************************************************************************
  */

#include "log_manager.h"

// Pointer to the uart2 handler
static UART_HandleTypeDef *huart_handler;

// Final log message buffer
static char buf[1024];

// Current global verbosity level
static enum VERBOSITY_LEVEL current_verbosity = VERBOSITY_INFO;

// Each verbosity level strings
static const char* verbosity_strings[] = { "Debug", "Info", "Warning", "Error" };


/**
  * @brief  Initialized the uart handler with the debug uart (uart2)
  *
  * @param  UART_HandleTypeDef *huart	debug uart handler
  * @retval None
  */
void init_log(UART_HandleTypeDef *huart)
{
	huart_handler = huart;
}

/**
  * @brief  Change log verbosity level
  *
  * @param  enum VERBOSITY_LEVEL verbosity	new verbosity to set.
  * @retval None
  */
void change_verbosity(enum VERBOSITY_LEVEL verbosity) {

	if (current_verbosity == verbosity){
		send_log(VERBOSITY_WARNING, "Current log verbosity is already %s. Request ignored", verbosity_strings[current_verbosity]);
	}
	else{
		current_verbosity = verbosity;
	}
}

/**
  * @brief  Send a log using debug uart. It use a blocking transmit.
  * 		Variadic function.
  *
  * @param  enum VERBOSITY_LEVEL verbosity	verbosity level of the log.
  * @param 	const char *message		message to send.
  * @param	... 	argument to format
  * @retval None
  */
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
