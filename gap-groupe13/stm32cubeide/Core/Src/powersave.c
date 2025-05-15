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
		//switch_sampling_rate();
		cmd_toggle_powersave_mode();
		//uart_disable(DEBUG_UART);

	}
	else {
		send_log(VERBOSITY_INFO, "Disabling power save mode");
		//switch_sampling_rate();
		cmd_toggle_powersave_mode();
		//uart_enable(DEBUG_UART);

	}
}
