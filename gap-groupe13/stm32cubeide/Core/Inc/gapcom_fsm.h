/**
  ******************************************************************************
  * @file    gapcom_fsm.h
  * @author  Group 13
  * @brief   Header for gapcom_fsm.c file.
  ******************************************************************************
  */


#ifndef SRC_GAPCOM_FSM_H_
#define SRC_GAPCOM_FSM_H_

#include "main.h"
#include "gapcom_impl.h"

// 1 (SOF) + 4 (ID) + 2 (LEN) + 2 (TYPE) + 2 (HEAD_CRC)
#define GAPCOM_HEADER_SIZE 9

// CRC16 = 2 bytes
#define GAPCOM_CRC_SIZE    2

// CRC16 = 2 bytes
#define GAPCOM_CRC_FOOTER_SIZE    2

/*
 * @brief	State of the parser.
 */
typedef enum {
    FSM_HEADER,
    FSM_PAYLOAD
} fsm_state_t;


/* Exported functions prototypes ---------------------------------------------*/
void gapcom_uart_fsm_init(UART_HandleTypeDef *huart, gapcom_handle_t *gapcom_handle);
void gapcom_uart_fsm_rx_callback();

#endif /* SRC_GAPCOM_FSM_H_ */
