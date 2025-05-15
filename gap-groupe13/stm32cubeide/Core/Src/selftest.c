/**
  ******************************************************************************
  * @file    selftest.c
  * @author  Group 13
  * @brief   Selftest for flash, debug uart, gyroscope and leds
  *
  ******************************************************************************
  */

#include "selftest.h"

// Pointer to the uart2 handler
static UART_HandleTypeDef *huart_handler;


/*
 * Static array for the final TAP output.
 */
static const char* top_delimiter = "============= TAP Output START ===============\r\n";
static const char* tap_version = "TAP version 14\r\n";

// Adapt when you are adding a test.
static const char* tap_tests_count = "1..4\r\n";
static const char* bottom_delimiter = "============= TAP Output END =================\r\n";
static char tap_flash_test_result[256];
static char tap_debug_uart_test_result[256];
static char tap_gyroscope_test_result[256];
static char tap_leds_test_result[256];


/**
  * @brief  Initialized the uart handler with the debug uart (uart2)
  *
  * @param  UART_HandleTypeDef *huart	debug uart handler
  * @retval None
  */
void init_test(UART_HandleTypeDef *huart)
{
	huart_handler = huart;
}

/**
  * @brief  Send the TAP final output.
  *
  * @param  None
  * @retval None
  */
void show_tap_output() {
	HAL_UART_Transmit(huart_handler, (uint8_t*)top_delimiter, (uint16_t)strlen(top_delimiter), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_version, (uint16_t)strlen(tap_version), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_tests_count, (uint16_t)strlen(tap_tests_count), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_flash_test_result, (uint16_t)strlen(tap_flash_test_result), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_debug_uart_test_result, (uint16_t)strlen(tap_debug_uart_test_result), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_gyroscope_test_result, (uint16_t)strlen(tap_gyroscope_test_result), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)tap_leds_test_result, (uint16_t)strlen(tap_leds_test_result), HAL_MAX_DELAY);
	HAL_UART_Transmit(huart_handler, (uint8_t*)bottom_delimiter, (uint16_t)strlen(bottom_delimiter), HAL_MAX_DELAY);
}


/**
  * @brief 	Send a message and wait for an input (y/n only)
  *
  * @param  const char *ask_msg		message to send before waiting for the input.
  * @retval return true if y, n else.
  */
static int _ask_user(const char *ask_msg) {
	uint8_t response;
	HAL_UART_Transmit(huart_handler, (uint8_t*)ask_msg, (uint16_t)strlen(ask_msg), HAL_MAX_DELAY);

	while (1) {
		if (HAL_UART_Receive(huart_handler, &response, 1, HAL_MAX_DELAY) == HAL_OK) {
			HAL_UART_Transmit(huart_handler, &response, 1, HAL_MAX_DELAY);
			HAL_UART_Transmit(huart_handler, (uint8_t*)"\r\n" , 2, HAL_MAX_DELAY);

			if (response == 'y') {
				return true;
			}
			else if (response == 'n') {
				return false;
			}
		}
	}


}


/**
 * ADD HERE THE TESTS YOU SHOULD RUN WHEN SELFTEST REQUEST RECEIVED.
 *
 * test_flash_access
 * test_uart_log_verbosity
 * test_gyroscope
 * test_leds
 */

GAPSelftestCode test_flash_access() {

    uint32_t *addr = (uint32_t *)TEST_FLASH_ADDR_OK;
    const uint32_t a = 0xDEADBEEF;
    uint32_t read_buffer[32];

    if (!flash_read_data((uint32_t)addr, read_buffer, sizeof(read_buffer))) {
    	strcpy(tap_flash_test_result, "not ok 1 - Flash read data failed\r\n");

        return GAPSelftestCode_GAP_SELFTEST_FLASH_FAILED_TO_READ;
    }

    if (!flash_write_data(addr, &a, sizeof(a))) {
    	strcpy(tap_flash_test_result, "not ok 1 - Flash write data failed\r\n");

        return GAPSelftestCode_GAP_SELFTEST_FLASH_FAILED_TO_WRITE;
    }

    if (!flash_read_data((uint32_t)addr, read_buffer, sizeof(read_buffer))) {
    	strcpy(tap_flash_test_result, "not ok 1 - Flash read data failed\r\n");

        return GAPSelftestCode_GAP_SELFTEST_FLASH_FAILED_TO_READ;
    }

    if (read_buffer[0] != a) {
    	strcpy(tap_flash_test_result, "not ok 1 - Flash read back is different\r\n");

        return GAPSelftestCode_GAP_SELFTEST_FLASH_READ_BACK_IS_DIFFERENT;
    }
	strcpy(tap_flash_test_result, "ok 1 - Internal flash write then read back\r\n");

    return GAPSelftestCode_GAP_SELFTEST_OK;
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
			strcpy(tap_debug_uart_test_result, "not ok 2 - User check failed when testing UART verbosity levels\r\n");
			return (GAPSelftestCode) i + 4;
		}

	}

	strcpy(tap_debug_uart_test_result, "ok 2 - UART verbosity levels\r\n");

	return GAPSelftestCode_GAP_SELFTEST_OK;

}


GAPSelftestCode test_gyroscope() {
	strcpy(tap_gyroscope_test_result, "not ok 3 - Gyroscope selftest not implemented\r\n");

	return GAPSelftestCode_GAP_SELFTEST_OK;
}





GAPSelftestCode test_leds() {

	const char *ask_off = "Please check LED. Is it off ? (y/n) ";
	const char *ask_on = "Please check LED. Is it on ? (y/n) ";

	LED_Off();

	if (!_ask_user(ask_off)){
		strcpy(tap_leds_test_result, "not ok 4 - User check failed when checking on led (OFF)\r\n");
		return GAPSelftestCode_GAP_SELTEST_LED_GREEN_NOK;
	}

	LED_On();

	if (!_ask_user(ask_on)){
		strcpy(tap_leds_test_result, "not ok 4 - User check failed when checking on led (ON)\r\n");
		LED_Off();
		return GAPSelftestCode_GAP_SELTEST_LED_GREEN_NOK;
	}



	strcpy(tap_leds_test_result, "ok 4 - LED\r\n");

	LED_Off();
	return GAPSelftestCode_GAP_SELFTEST_OK;

}
