/**
  ******************************************************************************
  * @file    version.h
  * @author  Group 13
  * @brief   Header for version.c file.
  ******************************************************************************
  */

#ifndef SRC_VERSION_H
#define SRC_VERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "utils/flash.h"
#include "log_manager.h"

// Max size of a version.
#define VERSION_SIZE GAPSetVersionReq_size

/* Exported functions prototypes ---------------------------------------------*/
bool is_valid_version(const char* version);
void get_version(char *buffer);
bool set_version(const char *new_version);

#endif // SRC_VERSION_H
