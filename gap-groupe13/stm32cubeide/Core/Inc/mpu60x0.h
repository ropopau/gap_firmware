/**
  ******************************************************************************
  * @file    mpu60x0.h
  * @author  Group 13
  * @brief   Header for mpu60x0.c file.
  ******************************************************************************
  */

#ifndef SRC_MPU60X0_H_
#define SRC_MPU60X0_H_

#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

/*
 *
 * @brief	Please look at the mpu60x0's datasheet.
 */
#define MPU6050_ADDR (0x68 << 1)
#define PWR_MGMT_1 0x6B
#define SMPLRT_DIV 0x19
#define CONFIG_REG 0x1A
#define FIFO_EN_REG 0x23
#define USER_CTRL_REG 0x6A
#define WAIT_WRITE_MPU_TIME 1000
#define FIFO_R_W_REG 0x74
#define FIFO_COUNTH_REG 0x72
#define I2C_MASTER 0x24
#define INT_ENABLE 0x38
#define GYRO_CONFIG 0x1B
#define GYRO_XOUT_H 0x43

/* Exported functions prototypes ---------------------------------------------*/
void init_mpu(I2C_HandleTypeDef *I2C_handler);
void set_gyroscope();
void unset_gyroscope();
void read_fifo();
void switch_sampling_rate();
void mpu_interrupt();


#endif /* SRC_MPU60X0_H_ */
