/*
 * gapclicmdcallback.h
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#ifndef SRC_GAPCLI_CMD_CALLBACK_H_
#define SRC_GAPCLI_CMD_CALLBACK_H_

#include "gapcom_core.h"
#include "gapcom/gapcom.h"
#include "gapcom/gapcom_server.h"

void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg);

#endif /* SRC_GAPCLI_CMD_CALLBACK_H_ */
