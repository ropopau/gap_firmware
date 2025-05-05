/*
 * mpu60x0.h
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#ifndef SRC_MPU60X0_H_
#define SRC_MPU60X0_H_

#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

#define MPU6050_ADDR (0x68 << 1)
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_PWR_MGMT_2 0x6C
#define MPU6050_SMPLRT_DIV 0x19
#define CONFIG_REG 0x1A
#define FIFO_EN_REG 0x23
#define USER_CTRL_REG 0x6A
#define WAIT_WRITE_MPU_TIME 2000
#define FIFO_R_W_REG 0x74
#define FIFO_COUNTL_REG 0x72
#define FIFO_COUNTH_REG 0x73
#define MPU6050_I2C_MASTER 0x24
#define GYRO_CONFIG 0x1B



void init_mpu(I2C_HandleTypeDef *I2C_handler);

HAL_StatusTypeDef set_gyroscope();

void read_fifo();

#endif /* SRC_MPU60X0_H_ */
