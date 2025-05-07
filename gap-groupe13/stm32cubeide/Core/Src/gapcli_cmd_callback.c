/*
 * gapclicmdcallback.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcli_cmd_callback.h"







void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	send_log(VERBOSITY_INFO, "ping request received");
	gapcom_respond_ping(handle, GAPErrorCode_GAP_OK);
}

void cmd_setverbosity_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	send_log(VERBOSITY_INFO, "set-log-verbosity request received");

	enum VERBOSITY_LEVEL new_log_level = *(enum VERBOSITY_LEVEL *)proto_msg;
	change_verbosity(new_log_level);
	gapcom_respond_set_log_verbosity(handle, GAPErrorCode_GAP_OK);

}

void cmd_setversion_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	send_log(VERBOSITY_INFO, "set-version request received");

	char *new_version = (char *)proto_msg;
	set_version(new_version);
	gapcom_respond_set_version(handle, GAPErrorCode_GAP_OK);

}

void cmd_getversion_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	send_log(VERBOSITY_INFO, "get-version request received");
	char version[16];
	get_version(version);

	gapcom_respond_get_version(handle, GAPErrorCode_GAP_OK, version);

}



void cmd_setgyroscope_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	send_log(VERBOSITY_DEBUG, "gyroscope");
	//set_gyroscope();
	gapcom_respond_set_gyroscope(handle, GAPErrorCode_GAP_OK);
}
