/*
 * log_manager.h
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#ifndef SRC_LOG_MANAGER_H_
#define SRC_LOG_MANAGER_H_

#include "main.h"

enum VERBOSITY_LEVEL {
    VERBOSITY_DEBUG,
    VERBOSITY_INFO,
    VERBOSITY_WARNING,
    VERBOSITY_ERROR
};

void init_log(UART_HandleTypeDef *huart);

void send_log(enum VERBOSITY_LEVEL, const char *message);

#endif /* SRC_LOG_MANAGER_H_ */
