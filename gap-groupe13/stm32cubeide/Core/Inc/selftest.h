/*
 * selftest.h
 *
 *  Created on: May 7, 2025
 *      Author: sanghyeon
 */

#ifndef SRC_SELFTEST_H
#define SRC_SELFTEST_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>
#include "log_manager.h"


void init_test(UART_HandleTypeDef *huart);


GAPSelftestCode test_uart_log_verbosity();


#endif // SRC_SELFTEST_H
