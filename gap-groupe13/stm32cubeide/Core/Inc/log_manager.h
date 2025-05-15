/**
  ******************************************************************************
  * @file    log_manager.h
  * @author  Group 13
  * @brief   Header for log_manager.c file.
  ******************************************************************************
  */

#ifndef SRC_LOG_MANAGER_H_
#define SRC_LOG_MANAGER_H_

#include "main.h"
#include <stdarg.h>


/*
 * @brief	Enum that define log verbosity.
 */
enum VERBOSITY_LEVEL {
    VERBOSITY_DEBUG,
    VERBOSITY_INFO,
    VERBOSITY_WARNING,
    VERBOSITY_ERROR
};


/* Exported functions prototypes ---------------------------------------------*/
void change_verbosity(enum VERBOSITY_LEVEL verbosity);
void init_log(UART_HandleTypeDef *huart);
void send_log(enum VERBOSITY_LEVEL, const char *message, ...);

#endif /* SRC_LOG_MANAGER_H_ */
