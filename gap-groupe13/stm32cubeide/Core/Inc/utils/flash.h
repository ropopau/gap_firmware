/**
  ******************************************************************************
  * @file    flash.h
  * @author  Group 13
  * @brief   Header for flash.c file.
  ******************************************************************************
  */

#ifndef SRC_FLASH_H
#define SRC_FLASH_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "log_manager.h"
#include <stdbool.h>
#include <string.h>

/* Exported functions prototypes ---------------------------------------------*/
bool flash_erase_sector(uint32_t sector);
bool flash_write_data(uint32_t * address, const void *data, uint32_t size);
bool flash_read_data(uint32_t address, void *data, uint32_t size);


#endif // SRC_FLASH_H
