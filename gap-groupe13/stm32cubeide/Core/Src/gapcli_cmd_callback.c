/**
  ******************************************************************************
  * @file    gapcli_cmd_callback.c
  * @author  Group 13
  * @brief   This file define every callbacks that will be used by gapcom.
  *
  * - Every callback triggered whenever gapcom receives a valid request.
  *
  * - If powersaving is enabled, every callback will do nothing
  * except for the power-save-mode request
  *
  ******************************************************************************
  */

#include "gapcli_cmd_callback.h"

// Tracking powersaving.
static bool is_powersaving = false;


/**
  * @brief  Function that enable powersaving mode.
  * if powersaving is true, every callback is ignored except the power-save-mode request's one.
  *
  * @param  None
  * @retval None
  */
void cmd_enable_powersave_mode() {
	is_powersaving = true;
}

/**
  * @brief  Function that disable powersaving mode.
  * if powersaving is true, every callback is ignored except the power-save-mode request's one.
  *
  * @param  None
  * @retval None
  */
void cmd_disable_powersave_mode() {
	is_powersaving = false;
}



/**
  * @brief  ping callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	empty
  * @retval None
  */
void cmd_ping_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	if (is_powersaving)
		return;
	if (proto_msg == NULL)
		send_log(VERBOSITY_ERROR, "Error of proto msg");
	send_log(VERBOSITY_INFO, "ping request received");
	gapcom_respond_ping(handle, GAPErrorCode_GAP_OK);
}

/**
  * @brief  set-log-verbosity callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	verbosity level
  * @retval None
  */
void cmd_setverbosity_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	GAPErrorCode error_code = GAPErrorCode_GAP_OK;
	if (is_powersaving)
		return;
	if (proto_msg == NULL)
		send_log(VERBOSITY_ERROR, "Error of proto msg");
	send_log(VERBOSITY_INFO, "set-log-verbosity request received");

	enum VERBOSITY_LEVEL new_log_level = *(enum VERBOSITY_LEVEL *)proto_msg;

	if (new_log_level > 4)
		error_code = GAPErrorCode_GAP_INVALID_LOG_VERBOSITY;
	else {
		send_log(
			VERBOSITY_INFO,
			"Verbosity has been succesfully changed"
		);
		change_verbosity(new_log_level);
	}
	gapcom_respond_set_log_verbosity(handle, error_code);
}

/**
  * @brief  set-version callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	new version to set
  * @retval None
  */
void cmd_setversion_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	GAPErrorCode error_code = GAPErrorCode_GAP_OK;
	if (is_powersaving)
		return;
	if (proto_msg == NULL)
		send_log(VERBOSITY_ERROR, "Error of proto msg");
	send_log(VERBOSITY_INFO, "set-version request received");

	const char *new_version = (const char *) proto_msg;

	if (!is_valid_version(new_version))
		error_code = GAPErrorCode_GAP_INVALID_VERSION_FORMAT;
	else {
		if (!set_version(new_version)) {
			send_log(VERBOSITY_ERROR, "Unknown error happened while setting version");
			return;
		}
	}
	gapcom_respond_set_version(handle, error_code);

}

/**
  * @brief  get-version callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	empty
  * @retval None
  */
void cmd_getversion_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	if (is_powersaving)
		return;
	if (proto_msg == NULL)
		send_log(VERBOSITY_ERROR, "Error of proto msg");
	send_log(VERBOSITY_INFO, "get-version request received");
	char version[16];


	get_version(version);

	gapcom_respond_get_version(handle, GAPErrorCode_GAP_OK, (const char*) version);


}

/**
  * @brief  selftest callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	empty
  * @retval None
  */
void cmd_selftest_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	if (is_powersaving)
		return;
	if (proto_msg == NULL)
		send_log(VERBOSITY_ERROR, "Error of proto msg");
	send_log(VERBOSITY_INFO, "selftest request received");

	GAPSelftestCode flash_res = test_flash_access();
	GAPSelftestCode debuguart_res = test_uart_log_verbosity();
	change_verbosity(VERBOSITY_INFO);
	GAPSelftestCode gyroscope_res = test_gyroscope();
	GAPSelftestCode leds_res = test_leds();

	gapcom_respond_selftest(handle, GAPErrorCode_GAP_OK, flash_res, debuguart_res, gyroscope_res, leds_res);

	show_tap_output();
}

/**
  * @brief  power-save-mode callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	enter or exit
  * @retval None
  */
void cmd_powersavemode_callback(gapcom_handle_t *handle, const void *proto_msg) {

	send_log(VERBOSITY_INFO, "powersavemode request received");

	enum POWERSAVE_REQ power_save_mode_type = *(enum POWERSAVE_REQ *)proto_msg;

	handle_powersavemode(power_save_mode_type);

	gapcom_respond_power_save_mode(handle, GAPErrorCode_GAP_OK);
}


/**
  * @brief  set-gyroscope callback
  *
  * @param  gapcom_handle_t *handle		gapcom handler
  * @param  const void *proto_msg 	on or off
  * @retval None
  */
void cmd_setgyroscope_callback(gapcom_handle_t *handle, const void *proto_msg)
{
	if (is_powersaving)
		return;
	const bool* is_set_gyroscope_addr = (const bool*) proto_msg;

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
