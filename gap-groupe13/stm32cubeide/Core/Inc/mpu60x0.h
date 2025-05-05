/*
 * mpu60x0.h
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#ifndef SRC_MPU60X0_H_
#define SRC_MPU60X0_H_

#include "main.h"

#define MPU6050_ADDR (0x68 << 1)
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_SMPLRT_DIV 0x19
#define CONFIG_REG 0x1A
#define FIFO_EN_REG 0x1A
#define USER_CTRL_REG 0x6A


HAL_StatusTypeDef init_mpu(I2C_HandleTypeDef *I2C_handler);

HAL_StatusTypeDef set_gyroscope();

#endif /* SRC_MPU60X0_H_ */
