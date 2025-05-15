/**
  ******************************************************************************
  * @file    powersave.c
  * @author  Group 13
  * @brief   Handling power-save-mode
  *
  ******************************************************************************
  */

#include <powersave.h>


/**
  * @brief  Handling powersave mode based on the request type
  *
  * @param  enum POWERSAVE_REQ req_type		Exit or Enter power-save-mode
  * @retval None
  */
void handle_powersavemode(enum POWERSAVE_REQ req_type) {
	if (req_type == ENTER){
		send_log(VERBOSITY_INFO, "Enabling power save mode");
		mpu_enable_power_save_mode();
		cmd_enable_powersave_mode();
		uart_disable(DEBUG_UART);

	}
	else {
		send_log(VERBOSITY_INFO, "Disabling power save mode");
		mpu_disable_power_save_mode();
		cmd_disable_powersave_mode();
		uart_enable(DEBUG_UART);

	}
}
