/*
 * log_manager.h
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#ifndef SRC_LOG_MANAGER_H_
#define SRC_LOG_MANAGER_H_

#include "main.h"
#include <stdarg.h>



enum VERBOSITY_LEVEL {
    VERBOSITY_DEBUG,
    VERBOSITY_INFO,
    VERBOSITY_WARNING,
    VERBOSITY_ERROR
};

static const char* verbosity_strings[] = { "Debug", "Info", "Warning", "Error" };




void change_verbosity(enum VERBOSITY_LEVEL verbosity);




void init_log(UART_HandleTypeDef *huart);

void send_log(enum VERBOSITY_LEVEL, const char *message, ...);

#endif /* SRC_LOG_MANAGER_H_ */
