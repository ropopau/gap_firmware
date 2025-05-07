/*
 * gapclicmdcallback.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcli_cmd_callback.h"


void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	send_log(VERBOSITY_DEBUG, "Ping");
	gapcom_respond_ping(handle, GAPErrorCode_GAP_OK);
}

void cmd_setverbosity_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	gapcom_respond_set_log_verbosity(handle, GAPErrorCode_GAP_OK);
}

void cmd_setgyroscope_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	bool* is_set_gyroscope_addr = (bool*)proto_msg;

	if (*is_set_gyroscope_addr == true)
	{
		send_log(VERBOSITY_DEBUG, "gyroscope set");
		set_gyroscope();
	}
	else
	{
		send_log(VERBOSITY_DEBUG, "gyroscope unset");
		unset_gyroscope();
	}
	gapcom_respond_set_gyroscope(handle, GAPErrorCode_GAP_OK);
}
