/*
 * gapcom_impl.c
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#include "gapcom_impl.h"

static UART_HandleTypeDef *huart2_handle;

static gapcom_handle_t *gapcom_handle_instance;

int close_sender(gapcom_sender_t *self)
{
	if (self == NULL)
		return -1;
	return 0;
}

int open_sender(gapcom_sender_t *self)
{
	if (self == NULL)
		return -1;
	return 0;
}

ssize_t send_sender(gapcom_sender_t *self, const uint8_t *buf, size_t len)
{
	if (self == NULL)
	{
		return -1;
	}
	HAL_StatusTypeDef status = HAL_UART_Transmit_IT(huart2_handle, buf, (uint16_t)len);
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

void create_gapcom_instance(UART_HandleTypeDef *handle)
{
	huart2_handle = handle;
	gapcom_handle_instance = gapcom_create();
	gapcom_set_sender_impl(gapcom_handle_instance, &gapcom_sender_t_impl);
	gapcom_install_callback(gapcom_handle_instance, cmd_ping_callback, GAPCOM_MSG_MIN);
	gapcom_install_callback(gapcom_handle_instance, cmd_setverbosity_callback, GAPCOM_MSG_SET_LOG_VERBOSITY_REQ);

	gapcom_install_callback(gapcom_handle_instance, cmd_setversion_callback, GAPCOM_MSG_SET_VERSION_REQ);
	gapcom_install_callback(gapcom_handle_instance, cmd_getversion_callback, GAPCOM_MSG_GET_VERSION_REQ);


	gapcom_install_callback(gapcom_handle_instance, cmd_setgyroscope_callback, GAPCOM_MSG_SET_GYROSCOPE_REQ);
	gapcom_uart_fsm_init(huart2_handle, gapcom_handle_instance);
}

