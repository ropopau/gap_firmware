/*
 * powersave.h
 *
 *  Created on: May 10, 2025
 *      Author: sanghyeon
 */

#ifndef SRC_POWERSAVE_H
#define SRC_POWERSAVE_H

#include <stdint.h>
#include <stdbool.h>
#include "log_manager.h"
#include "mpu60x0.h"
#include "utils/uart.h"

enum POWERSAVE_REQ {
    EXIT,
    ENTER
};


void handle_powersavemode(enum POWERSAVE_REQ req_type);

#endif // SRC_POWERSAVE_H
