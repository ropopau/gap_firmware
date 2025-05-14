/**
  ******************************************************************************
  * @file    gapcom_impl.h
  * @author  Group 13
  * @brief   Header for gapcom_impl.c file.
  ******************************************************************************
  */

#ifndef SRC_GAPCOM_IMPL_H_
#define SRC_GAPCOM_IMPL_H_

#include "communicator.h"
#include "communicator_impl_server.h"
#include "gapcom_core.h"
#include "gapcom/gapcom.h"
#include "gapcom/gapcom_server.h"
#include "gapcli_cmd_callback.h"
#include "main.h"

/* Exported functions prototypes ---------------------------------------------*/
void create_gapcom_instance(UART_HandleTypeDef *handle);


#endif /* SRC_GAPCOM_IMPL_H_ */
