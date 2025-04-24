/*
 * gapcom_impl.h
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#ifndef SRC_GAPCOM_IMPL_H_
#define SRC_GAPCOM_IMPL_H_

#include "communicator.h"
#include "communicator_impl_server.h"
#include "gapcom_core.h"
#include "gapcom/gapcom.h"
#include "gapcom/gapcom_server.h"
#include "main.h"

void create_gapcom_instance(UART_HandleTypeDef *huart2_handle);

void receive_gapcom_incoming_uart_message(uint8_t *buf, uint16_t size);

void send_respond_ping();

#endif /* SRC_GAPCOM_IMPL_H_ */
