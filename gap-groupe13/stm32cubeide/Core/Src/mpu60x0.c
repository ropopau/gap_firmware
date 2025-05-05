/*
 * mpu60x0.c
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#include "mpu60x0.h"

static I2C_HandleTypeDef *I2C_handler_instance;

HAL_StatusTypeDef init_mpu(I2C_HandleTypeDef *I2C_handler)
{
	I2C_handler_instance = I2C_handler;
}

HAL_StatusTypeDef set_gyroscope()
{
	//Leave "Mode Veille"
	uint8_t data;
	data = 0x00;
	if (HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		   send_log(VERBOSITY_ERROR, "Failed to leave Sleep mode");
		   return HAL_ERROR;
	}
	HAL_Delay(100);

	// 20Hz -> SMPLRT_DIV = 49 (1000 / (1+49) = 20Hz)
	data = 49;
	if (HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_SMPLRT_DIV, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
	   send_log(VERBOSITY_ERROR, "Failed to leave set SMPLRT_DIV to 20Hz");
	   return HAL_ERROR;
	}
	HAL_Delay(10);

	// DLPF: 42Hz Bandwidth
	data = 0x03;
	if (HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		send_log(VERBOSITY_ERROR, "Failed to set DLPF 42Hz");
		return HAL_ERROR;
	}

	//Enable FIFO only for gyroscope
	data = 0x70; // XG | YG | ZG
	if (HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		send_log(VERBOSITY_ERROR, "Failed to Enable FIFO for gyroscope");
		return HAL_ERROR;
	}

	//Enable FIFO in USER_CRTL
	data = 0x40;
	if (HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		send_log(VERBOSITY_ERROR, "Failed to leave Sleep mode");
		return HAL_ERROR;
	}
	return HAL_OK;
}
