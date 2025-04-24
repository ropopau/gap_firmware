/*
 * gapcom_impl.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcom_impl.h"

static USART_HandleTypeDef *husart2_handle;

static gapcom_handle_t *gapcom_handle_instance;

int close_sender(gapcom_sender_t *self)
{
	return 0;
}

int open_sender(gapcom_sender_t *self)
{
	return 0;
}

ssize_t send_sender(gapcom_sender_t *self, const uint8_t *buf, size_t len)
{
	HAL_StatusTypeDef status = HAL_USART_Transmit_IT(husart2_handle, buf, len);
    if (status == HAL_OK) {
        return (ssize_t)len;
    } else {
        return -1;
    }}

gapcom_sender_t gapcom_sender_t_impl = {
		.close = close_sender,
		.open = open_sender,
		.send = send_sender
};

void create_gapcom_instance(USART_HandleTypeDef *handle)
{
	husart2_handle = handle;
	gapcom_handle_instance = gapcom_create();
	gapcom_set_sender_impl(gapcom_handle_instance, &gapcom_sender_t_impl);
}

void receive_gapcom_incoming_uart_message(uint8_t *buf, uint16_t size)
{
	gapcom_accept(gapcom_handle_instance, buf, size);
	gapcom_respond_ping(gapcom_handle_instance, GAPErrorCode_GAP_OK);
}

void send_respond_ping()
{
	gapcom_respond_ping(gapcom_handle_instance, GAPErrorCode_GAP_OK);
}

