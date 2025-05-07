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


void cmd_selftest_callback(gapcom_handle_t *handle, const void *proto_msg)
{

	send_log(VERBOSITY_INFO, "selftest request received");
	char version[16];
	get_version(version);

	GAPSelftestCode debuguart_res = test_uart_log_verbosity();

	gapcom_respond_selftest(handle, GAPErrorCode_GAP_OK, 0, debuguart_res, 0, 0);

	send_log(VERBOSITY_INFO, "Selftest ended. Look at the control console to view results.");
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
