/**
  ******************************************************************************
  * @file    powersave.h
  * @author  Group 13
  * @brief   Header for powersave.c file.
  ******************************************************************************
  */

#ifndef SRC_POWERSAVE_H
#define SRC_POWERSAVE_H

#include <stdint.h>
#include <stdbool.h>
#include "log_manager.h"
#include "mpu60x0.h"
#include "utils/uart.h"

/*
 * @brief	power-save-mode request type.
 */
enum POWERSAVE_REQ {
    EXIT,
    ENTER
};

/* Exported functions prototypes ---------------------------------------------*/
void handle_powersavemode(enum POWERSAVE_REQ req_type);

#endif // SRC_POWERSAVE_H
