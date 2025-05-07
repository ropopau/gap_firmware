/*
 * gapcomfsm.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcom_fsm.h"

static UART_HandleTypeDef *gapcom_uart;
static gapcom_handle_t *gapcom_handle_instance;
static fsm_state_t fsm_state = FSM_HEADER;
static char header_buf[11];
static char payload_buf[1024];
static uint16_t expected_payload_size = 0;

void gapcom_uart_fsm_init(UART_HandleTypeDef *huart, gapcom_handle_t *gapcom_handle)
{
	 gapcom_handle_instance = gapcom_handle;
	 gapcom_uart = huart;
	 fsm_state = FSM_HEADER;
	 HAL_UART_Receive_IT(gapcom_uart, (uint8_t*)header_buf, GAPCOM_HEADER_SIZE);
}

void gapcom_uart_fsm_rx_callback()
{
	if (fsm_state == FSM_HEADER)
	{
		 uint16_t len = (header_buf[5] << 8) | header_buf[6];
		 fsm_state = FSM_PAYLOAD;
		 expected_payload_size = len + GAPCOM_CRC_SIZE;
		 if (len != 0)
		 {
			 expected_payload_size += GAPCOM_CRC_FOOTER_SIZE; // IF PAYLOAD ISN'T EMPTY, WE HAD THE FOOTER CHECKSUM
		 }
		 HAL_UART_Receive_IT(gapcom_uart, (uint8_t*)payload_buf, expected_payload_size);
	}
	else
	{
		 char total_buf[1035];
		 memcpy(total_buf, header_buf, GAPCOM_HEADER_SIZE);
		 memcpy(total_buf + GAPCOM_HEADER_SIZE, payload_buf, expected_payload_size);
		 size_t total_size = GAPCOM_HEADER_SIZE + expected_payload_size;
		 char buf[1024];
		 sprintf(buf, "%d", total_size);
		 gapcom_accept(gapcom_handle_instance, (uint8_t*)total_buf, total_size);
		 fsm_state = FSM_HEADER;
		 HAL_UART_Receive_IT(gapcom_uart, (uint8_t*)header_buf, GAPCOM_HEADER_SIZE);
	}
}
