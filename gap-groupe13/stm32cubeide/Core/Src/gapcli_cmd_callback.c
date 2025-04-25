/*
 * gapclicmdcallback.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcli_cmd_callback.h"


void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	gapcom_respond_ping(handle, GAPErrorCode_GAP_OK);
}

void cmd_setverbosity_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	gapcom_respond_set_log_verbosity(handle, GAPErrorCode_GAP_OK);
}
