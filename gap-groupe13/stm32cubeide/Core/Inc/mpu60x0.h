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

void init_mpu(I2C_HandleTypeDef *I2C_handler);

void set_gyroscope();

void unset_gyroscope();

void configure_for_selftest();

bool validate_selftest_x(int16_t X_axis);

bool validate_selftest_y(int16_t Y_axis);

bool validate_selftest_z(int16_t Z_axis);


void read_fifo();

void switch_sampling_rate();

void mpu_interrupt();


#endif /* SRC_MPU60X0_H_ */
