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
#include "mpu60x0.h"
#include "log_manager.h"


#define TEST_FLASH_ADDR_OK  0x08060000


void init_test(UART_HandleTypeDef *huart);

void show_tap_output();


GAPSelftestCode test_flash_access();
GAPSelftestCode test_uart_log_verbosity();
GAPSelftestCode test_gyroscope();
GAPSelftestCode test_leds();


#endif // SRC_SELFTEST_H
