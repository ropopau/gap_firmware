/*
 * selftest.c
 *
 *  Created on: May 7, 2025
 *      Author: sanghyeon
 */

#include "selftest.h"

UART_HandleTypeDef *huart_handler_test;

void init_test(UART_HandleTypeDef *huart)
{
	huart_handler_test = huart;
}


int _ask_user(char *ask_msg) {
	uint8_t response;
	HAL_UART_Transmit(huart_handler_test, (uint8_t*)ask_msg, strlen(ask_msg), HAL_MAX_DELAY);

	while (1) {
		if (HAL_UART_Receive(huart_handler_test, &response, 1, HAL_MAX_DELAY) == HAL_OK) {
			HAL_UART_Transmit(huart_handler_test, &response, 1, HAL_MAX_DELAY);
			HAL_UART_Transmit(huart_handler_test, (uint8_t*)"\r\n" , 2, HAL_MAX_DELAY);

			if (response == 'y') {
				return true;
			}
			else if (response == 'n') {
				return false;
			}
			else {
				// Invalid input - ask again
				HAL_UART_Transmit(huart_handler_test, (uint8_t*)"Please answer y or n: ", 21, HAL_MAX_DELAY);
			}
		}
	}


}



GAPSelftestCode test_uart_log_verbosity() {

	const char *ask_debug = "Please check logs above. Expecting levels : Debug, Info, Warning, Error. OK ? (y/n) ";
	const char *ask_info = "Please check logs above. Expecting levels : Info, Warning, Error. OK ? (y/n) ";
	const char *ask_warning = "Please check logs above. Expecting levels : Warning, Error. OK ? (y/n) ";
	const char *ask_error = "Please check logs above. Expecting levels : Error. OK ? (y/n) ";


	const char *ask_lists[4] = {ask_debug, ask_info, ask_warning, ask_error};



	for (int i = 0; i < 4; i++) {
		change_verbosity((enum VERBOSITY_LEVEL) i);
		send_log(VERBOSITY_DEBUG, "Testing GAP");
		send_log(VERBOSITY_INFO, "Testing GAP");
		send_log(VERBOSITY_WARNING, "Testing GAP");
		send_log(VERBOSITY_ERROR, "Testing GAP");

		if (!_ask_user(ask_lists[i])){
			return (GAPSelftestCode) i + 4;
		}

	}

	change_verbosity(VERBOSITY_INFO);


	return GAPSelftestCode_GAP_SELFTEST_OK;

}
