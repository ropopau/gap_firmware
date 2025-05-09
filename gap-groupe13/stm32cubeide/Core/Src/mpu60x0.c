/*
 * mpu60x0.c
 *
 *  Created on: Apr 25, 2025
 *      Author: valentin
 */

#include "mpu60x0.h"

static I2C_HandleTypeDef *I2C_handler_instance;
static bool is_active = false;
static bool set_gyroscope_d = false;

void init_mpu(I2C_HandleTypeDef *I2C_handler)
{
	I2C_handler_instance = I2C_handler;
	//RESET MPU
	uint8_t reset = 0x80;
	HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &reset, 1, WAIT_WRITE_MPU_TIME);
	HAL_Delay(200);
}

void set_gyroscope()
{
	set_gyroscope_d = true;
}

void unset_gyroscope()
{
	set_gyroscope_d = false;
	//RESET MPU
	uint8_t reset = 0x80;
	HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &reset, 1, WAIT_WRITE_MPU_TIME);
}

HAL_StatusTypeDef config_mpu() {
    uint8_t data;
    HAL_StatusTypeDef status;

    HAL_Delay(200);

    // Wake up
    data = 0x01;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to wake up device");
        return status;
    }
    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Set sample rate to 100Hz (1kHz / (1+49))
    data = 49;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to set sample rate");
        return status;
    }
    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Set FIFO_EN to gyro bits
    data = 0x70;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to enable gyro FIFO");
        return status;
    }

    // Enable FIFO operation
    data = 0x40;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to enable FIFO operation");
        return status;
    }

    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Set DLPF_CFG to 3 (42Hz bandwidth)
    data = 0x03;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to set DLPF");
        return status;
    }

    // Disable I2C master mode
    data = 0x00;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_I2C_MASTER, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to disable I2C master mode");
        return status;
    }

    send_log(VERBOSITY_INFO, "MPU6050 FIFO configured successfully");
    is_active = true;

    HAL_Delay(WAIT_WRITE_MPU_TIME);

    return HAL_OK;
}


void read_fifo() {
    if (set_gyroscope_d) {
        config_mpu();
        set_gyroscope_d = false;
        return;
    }

    if (!is_active) {
        return;
    }

    // First check if device is still responding
    if (HAL_I2C_IsDeviceReady(I2C_handler_instance, MPU6050_ADDR, 1, 100) != HAL_OK) {
        send_log(VERBOSITY_ERROR, "MPU6050 not responding during read");
        is_active = false;
        return;
    }

    // Read FIFO count - this tells us how many bytes are in the FIFO
    uint8_t fifo_count_buf[2];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        I2C_handler_instance,
        MPU6050_ADDR,
        FIFO_COUNTH_REG,
        1,
        fifo_count_buf,
        2,
        WAIT_WRITE_MPU_TIME
    );

    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to read FIFO count");
        return;
    }

    uint16_t fifo_count = ((uint16_t)fifo_count_buf[0] << 8) | fifo_count_buf[1];

    // FIFO count too small
    if (fifo_count < 6) {
        return;
    }

    uint8_t data[6];
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, FIFO_R_W_REG, 1, data, 6, WAIT_WRITE_MPU_TIME);

    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to read FIFO data");
        return;
    }

    int16_t x = (int16_t)((data[0] << 8) | data[1]);
    int16_t y = (int16_t)((data[2] << 8) | data[3]);
    int16_t z = (int16_t)((data[4] << 8) | data[5]);

    char buf[64];
    sprintf(buf, "%c%05d %c%05d %c%05d\r\n",
        (x < 0 ? '-X' : 'X'), abs(x),
        (y < 0 ? '-Y' : 'Y'), abs(y),
        (z < 0 ? '-Z' : 'Z'), abs(z)
    );
    send_log(VERBOSITY_INFO, buf);

    HAL_Delay(50);
}
