/*
 * gapcomfsm.h
 *
 *  Created on: Apr 24, 2025
 *      Author: valentin
 */

#ifndef SRC_GAPCOM_FSM_H_
#define SRC_GAPCOM_FSM_H_

#include "main.h"
#include "gapcom_impl.h"

#define GAPCOM_HEADER_SIZE 9    // Calculé : 1 (SOF) + 4 (ID) + 2 (LEN) + 2 (TYPE) + 2 (HEAD_CRC)
#define GAPCOM_CRC_SIZE    2     // CRC16 = 2 octets
#define GAPCOM_CRC_FOOTER_SIZE    2     // CRC16 = 2 octets


typedef enum {
    FSM_HEADER,
    FSM_PAYLOAD
} fsm_state_t;

void gapcom_uart_fsm_init(UART_HandleTypeDef *huart, gapcom_handle_t *gapcom_handle);
void gapcom_uart_fsm_rx_callback();

#endif /* SRC_GAPCOM_FSM_H_ */
