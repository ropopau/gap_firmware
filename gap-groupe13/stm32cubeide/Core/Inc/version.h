/*
 * version.h
 *
 *  Created on: May 5, 2025
 *      Author: sanghyeon
 */

#ifndef SRC_VERSION_H
#define SRC_VERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "utils/flash.h"
#include "log_manager.h"


#define VERSION_SIZE GAPSetVersionReq_size

bool is_valid_version(const char* version);

void get_version(char *buffer);

bool set_version(const char *new_version);

#endif // SRC_VERSION_H
