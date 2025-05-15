/**
  ******************************************************************************
  * @file    mpu60x0.c
  * @author  Group 13
  * @brief   This file defines all supports functions for mpu
  *
  * - init mpu, set and unset reading gyroscope
  *
  * - Callback called by interrupt
  *
  * - SelfTest
  *
  * -Read FIFO
  *
  ******************************************************************************
  */

#include "mpu60x0.h"

static I2C_HandleTypeDef *I2C_handler_instance;
static bool is_active = false;
static bool set_gyroscope_d = false;
static bool is_powersaving = false;

/**
  * @brief  Initialized mpu60x0.c
  *
  * @param  I2C_HandleTypeDef *I2C_handler i2c Handler
  * @retval None
  */
void init_mpu(I2C_HandleTypeDef *I2C_handler)
{
	I2C_handler_instance = I2C_handler;
	//RESET MPU
	uint8_t reset = 0x80;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, PWR_MGMT_1, 1, &reset, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to init mpu");
    }
	HAL_Delay(200);
}

/**
  * @brief  Set gyroscope
  *
  * @param  None
  * @retval None
  */
void set_gyroscope()
{
	set_gyroscope_d = true;
}


/**
  * @brief  Unset gyroscope (disable reading + reset mpu)
  *
  * @param  None
  * @retval None
  */
void unset_gyroscope()
{
	set_gyroscope_d = false;
	//RESET MPU
	uint8_t reset = 0x80;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, PWR_MGMT_1, 1, &reset, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to unset gyroscope");
    }
}

/**
  * @brief  Configure MPU (Wake Up, DLPF, Set Sample Rate, Enable FIFO, Enable Interrupt...)
  *
  * @param  None
  * @retval HAL_StatusTypeDef
  */
static HAL_StatusTypeDef config_mpu() {
    uint8_t data;
    HAL_StatusTypeDef status;

    HAL_Delay(200);

    // Wake up
    data = 0x01;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to wake up device");
        return status;
    }
    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Set sample rate to 100Hz (1kHz / (1+49))
    data = 49;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to set sample rate");
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
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, I2C_MASTER, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to disable I2C master mode");
        return status;
    }

    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Enable FIFO operation
    data = 0x40;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to enable FIFO operation");
        return status;
    }

    // Set Interrupt enable on FIFO OVERFLOW
    data = 0x10;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, INT_ENABLE, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
    	send_log(VERBOSITY_ERROR, "Failed enable int interrupt");
    }

    HAL_Delay(WAIT_WRITE_MPU_TIME);

    // Set FIFO_EN to gyro bits
   data = 0x70;
   status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
   if (status != HAL_OK) {
	   send_log(VERBOSITY_ERROR, "Failed to enable gyro FIFO");
	   return status;
   }

    send_log(VERBOSITY_INFO, "MPU6050 FIFO configured successfully");
    is_active = true;

    HAL_Delay(WAIT_WRITE_MPU_TIME);

    return HAL_OK;
}

/**
  * @brief  Read fifo if set gyroscope is enabled
  *
  * @param  None
  * @retval None
  */
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
    sprintf(buf, "%s%05d %s%05d %s%05d\r\n",
        (x < 0 ? "-X" : "X"), abs(x),
        (y < 0 ? "-Y" : "Y"), abs(y),
        (z < 0 ? "-Z" : "Z"), abs(z)
    );
    send_log(VERBOSITY_INFO, buf);
}

static int nth_interrupt = 0;


void mpu_disable_power_save_mode() {
	if (!set_gyroscope_d)
		return;

	uint8_t data;
	HAL_StatusTypeDef status;

	is_powersaving = true;
	// Set sample rate to 5Hz (1kHz / (1+199))
	data = 199;


	status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
	if (status != HAL_OK) {
		send_log(VERBOSITY_ERROR, "Failed to set sample rate");
		return;
	}
}

void mpu_enable_power_save_mode(){
	if (!set_gyroscope_d)
		return;

	uint8_t data;
	HAL_StatusTypeDef status;

	is_powersaving = false;
	// Set sample rate to 20Hz (1kHz / (1+49))
	data = 49;


	status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
	if (status != HAL_OK) {
		send_log(VERBOSITY_ERROR, "Failed to set sample rate");
		return;
	}
}


/**
  * @brief  MPU interrupt, called when FIFO is full
  *
  * @param  None
  * @retval None
  */
void mpu_interrupt()
{
	send_log(VERBOSITY_ERROR, "FIFO full, be careful !");
	nth_interrupt += 1;
	if (nth_interrupt >= 5)
	{
		send_log(VERBOSITY_ERROR, "FIFO was fulled 5 times, gyroscope unset...");
		unset_gyroscope();
		nth_interrupt = 0;
	}
}
