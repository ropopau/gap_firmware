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
	uint8_t reset = 0x80;
	//RESET MPU
	HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &reset, 1, WAIT_WRITE_MPU_TIME);
	HAL_Delay(100);
}

HAL_StatusTypeDef set_gyroscope()
{
	set_gyroscope_d = true;
	return HAL_OK;
}


HAL_StatusTypeDef config_mpu() {
    uint8_t data;
    HAL_StatusTypeDef status;

    // 1. Verify the device is responding
    if (HAL_I2C_IsDeviceReady(I2C_handler_instance, MPU6050_ADDR, 3, WAIT_WRITE_MPU_TIME) != HAL_OK) {
        send_log(VERBOSITY_ERROR, "MPU6050 not responding, check connection");
        return HAL_ERROR;
    }

    send_log(VERBOSITY_DEBUG, "MPU6050 device ready");

    // 2. Reset the device completely
    data = 0x80;  // Reset bit
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to reset MPU6050");
        return status;
    }

    // Wait for reset to complete
    HAL_Delay(100);

    // 3. Wake up the device (clear sleep bit)
    data = 0x01;  // Set clock source to PLL with X-axis gyro reference
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to wake up MPU6050");
        return status;
    }
    HAL_Delay(50);

    // Verify the device ID
    uint8_t id;
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, 0x75, 1, &id, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK || id != 0x68) {
        send_log(VERBOSITY_ERROR, "Invalid device ID or read failure");
        return HAL_ERROR;
    }

    send_log(VERBOSITY_DEBUG, "MPU6050 ID verified");

    // 4. Configure sample rate divider
    data = 9;  // 100Hz (1000Hz / (1+9) = 100Hz)
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to set sample rate");
        return status;
    }

    // 5. Configure DLPF
    data = 0x02;  // 94Hz bandwidth, 3ms delay
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to configure DLPF");
        return status;
    }

    // 6. Configure gyroscope range to ±250°/s
    data = 0x00;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, GYRO_CONFIG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to configure gyroscope range");
        return status;
    }

    // 7. Disable accelerometer by putting it to standby
    data = 0x38;  // Disable XA, YA, ZA accelerometer
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_2, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to disable accelerometer");
        return status;
    }

    // 8. Prepare for FIFO operation - first disable everything
    data = 0x00;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to clear USER_CTRL");
        return status;
    }
    HAL_Delay(50);

    // 9. Reset FIFO
    data = 0x04;  // Reset FIFO
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to reset FIFO");
        return status;
    }
    HAL_Delay(50);

    // 10. Enable FIFO operation
    data = 0x40;  // Enable FIFO
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to enable FIFO");
        return status;
    }

    // 11. Configure which data to store in FIFO
    data = 0x70;  // Store gyro data (XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN)
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to configure FIFO data selection");
        return status;
    }

    send_log(VERBOSITY_INFO, "MPU6050 configuration complete - FIFO enabled for gyroscope data");
    is_active = true;

    // Wait for FIFO to fill with some data
    HAL_Delay(500);

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

    // Combine the high and low bytes to get the count
    uint16_t fifo_count = ((uint16_t)fifo_count_buf[0] << 8) | fifo_count_buf[1];

    // Log the FIFO count for debugging
    char count_msg[64];
    sprintf(count_msg, "FIFO count: %d bytes", fifo_count);
    send_log(VERBOSITY_DEBUG, count_msg);

    // If FIFO count is zero or too small, wait for more data
    if (fifo_count < 6) {
        send_log(VERBOSITY_DEBUG, "Waiting for more data in FIFO");

        // Try reading gyro registers directly as a test
        uint8_t raw_gyro[6];
        if (HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, 0x43, 1, raw_gyro, 6, 100) == HAL_OK) {
            int16_t gx = (int16_t)((raw_gyro[0] << 8) | raw_gyro[1]);
            int16_t gy = (int16_t)((raw_gyro[2] << 8) | raw_gyro[3]);
            int16_t gz = (int16_t)((raw_gyro[4] << 8) | raw_gyro[5]);

            char direct_msg[64];
            sprintf(direct_msg, "Direct read: X:%d Y:%d Z:%d", gx, gy, gz);
            send_log(VERBOSITY_INFO, direct_msg);
        }

        // Reset FIFO to try again
        uint8_t reset_cmd = 0x04;  // Reset FIFO
        HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &reset_cmd, 1, WAIT_WRITE_MPU_TIME);
        HAL_Delay(10);

        // Re-enable FIFO
        uint8_t enable_cmd = 0x40;  // Enable FIFO
        HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &enable_cmd, 1, WAIT_WRITE_MPU_TIME);

        return;
    }

    // Calculate how many complete sets of gyro data (each set is 6 bytes)
    int complete_sets = fifo_count / 6;

    // For this example, we'll just read the most recent complete set
    // If we wanted all data, we would need to loop and read 'complete_sets' times

    // Position to the last complete set
    int bytes_to_skip = (complete_sets - 1) * 6;

    // If there are multiple sets, we need to read and discard older data
    if (bytes_to_skip > 0) {
        for (int i = 0; i < bytes_to_skip; i += 6) {
            uint8_t temp[6];
            status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, FIFO_R_W_REG, 1, temp, 6, WAIT_WRITE_MPU_TIME);
            if (status != HAL_OK) {
                send_log(VERBOSITY_ERROR, "Failed to skip old FIFO data");
                return;
            }
        }
    }

    // Now read the latest set of data
    uint8_t data[6];
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, FIFO_R_W_REG, 1, data, 6, WAIT_WRITE_MPU_TIME);

    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to read FIFO data");
        return;
    }

    // Extract the gyroscope values
    int16_t x = (int16_t)((data[0] << 8) | data[1]);
    int16_t y = (int16_t)((data[2] << 8) | data[3]);
    int16_t z = (int16_t)((data[4] << 8) | data[5]);

    // Format and send the data
    char buf[64];
    sprintf(buf, "%c%05d %c%05d %c%05d\r\n",
        (x < 0 ? '-' : 'X'), abs(x),
        (y < 0 ? '-' : 'Y'), abs(y),
        (z < 0 ? '-' : 'Z'), abs(z)
    );
    send_log(VERBOSITY_INFO, buf);

    // If FIFO is getting full, reset it to prevent overflow
    if (fifo_count > 500) {
        uint8_t reset_cmd = 0x04;  // Reset FIFO
        HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &reset_cmd, 1, WAIT_WRITE_MPU_TIME);
        HAL_Delay(10);

        // Re-enable FIFO
        uint8_t enable_cmd = 0x40;  // Enable FIFO
        HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &enable_cmd, 1, WAIT_WRITE_MPU_TIME);
    }

    // Give the FIFO time to fill again
    HAL_Delay(50);
}
