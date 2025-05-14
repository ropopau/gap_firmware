/*
 * powersave.c
 *
 *  Created on: May 10, 2025
 *      Author: sanghyeon
 */

#include <powersave.h>


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
