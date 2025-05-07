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
#include <stdbool.h>
#include "log_manager.h"


#define VERSION_SIZE 16


void get_version(uint8_t *buffer);

void set_version(const uint8_t *new_version);

#endif // SRC_VERSION_H
