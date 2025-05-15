/**
  ******************************************************************************
  * @file    gapcli_cmd_callback.h
  * @author  Group 13
  * @brief   Header for gapcli_cmd_callback.c file.
  ******************************************************************************
  */

#ifndef SRC_GAPCLI_CMD_CALLBACK_H_
#define SRC_GAPCLI_CMD_CALLBACK_H_

#include <stdbool.h>

#include "main.h"

#include "gapcom_core.h"
#include "gapcom/gapcom.h"
#include "gapcom/gapcom_server.h"
#include "version.h"
#include "log_manager.h"
#include "powersave.h"



/* Exported functions prototypes ---------------------------------------------*/
void cmd_toggle_powersave_mode();
void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_setverbosity_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_setversion_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_getversion_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_selftest_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_powersavemode_callback(gapcom_handle_t *handle, const void *proto_msg);
void cmd_setgyroscope_callback(gapcom_handle_t *handle, const void *proto_msg);


#endif /* SRC_GAPCLI_CMD_CALLBACK_H_ */
