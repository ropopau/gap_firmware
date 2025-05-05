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
	HAL_Delay(200);
}

HAL_StatusTypeDef set_gyroscope()
{
	set_gyroscope_d = true;
	return HAL_OK;
}


// MPU6050 FIFO-specific fix
// This approach focuses on the minimal necessary configuration to get the FIFO working

HAL_StatusTypeDef config_mpu() {
    uint8_t data;
    HAL_StatusTypeDef status;

    // Wait for reset to complete - this is critical
    HAL_Delay(200);

    // 2. Check the device ID to verify communication
    uint8_t id;
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, 0x75, 1, &id, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to read WHO_AM_I register");
        return status;
    }

    if (id != 0x68) {
        send_log(VERBOSITY_ERROR, "Invalid device ID: 0x%02X, expected 0x68");
        return HAL_ERROR;
    }

    send_log(VERBOSITY_DEBUG, "MPU6050 ID verified: 0x68");

    // 3. Wake up by clearing the sleep bit and setting clock source
    data = 0x01;  // Clear sleep bit (bit 6) and set clock source to PLL with X-axis gyro reference
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to wake up device");
        return status;
    }
    HAL_Delay(1000);

    // 4. Verify that sleep mode is disabled by reading the register back
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK || (data & 0x40)) {  // Check bit 6 (sleep bit)
        send_log(VERBOSITY_ERROR, "Device still in sleep mode");
        return HAL_ERROR;
    }

    // 5. Set sample rate to 100Hz (1kHz / (1+9))
    data = 9;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_SMPLRT_DIV, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to set sample rate");
        return status;
    }
    HAL_Delay(1000);

    // 6. Configure digital low pass filter
    data = 0x03;  // ~42Hz bandwidth
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to configure DLPF");
        return status;
    }
    HAL_Delay(1000);

    // 7. Configure gyroscope (±250°/s)
    data = 0x00;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, GYRO_CONFIG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to configure gyroscope");
        return status;
    }
    HAL_Delay(1000);

    // 8. Disable accelerometer to save power and potentially reduce conflicts
    data = 0x38;  // Set standby bits for all accelerometer axes
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, MPU6050_PWR_MGMT_2, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to disable accelerometer");
        return status;
    }
    HAL_Delay(1000);

    // CRITICAL: The following order for FIFO setup is important

    // 9. Disable FIFO
    data = 0x00;  // Disable FIFO, I2C master, etc.
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to disable FIFO");
        return status;
    }
    HAL_Delay(1000);

    // 10. Reset FIFO
    data = 0x04;  // FIFO reset bit
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to reset FIFO");
        return status;
    }
    HAL_Delay(1000);

    // 11. Clear the FIFO configuration register
    data = 0x00;
    status = HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK) {
        send_log(VERBOSITY_ERROR, "Failed to clear FIFO_EN register");
        return status;
    }
    HAL_Delay(1000);

    // 11. Set FIFO_EN to gyro bits
    data = 0x70;  // XG, YG, ZG
    HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);

    // 12. Enable FIFO operation
    data = 0x40;
    HAL_I2C_Mem_Write(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);

    HAL_Delay(1000);

    // 14. Verify the FIFO is enabled by reading back registers
    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, USER_CTRL_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK || !(data & 0x40)) {
        send_log(VERBOSITY_ERROR, "FIFO not enabled in USER_CTRL");
        return HAL_ERROR;
    }
    HAL_Delay(1000);

    status = HAL_I2C_Mem_Read(I2C_handler_instance, MPU6050_ADDR, FIFO_EN_REG, 1, &data, 1, WAIT_WRITE_MPU_TIME);
    if (status != HAL_OK || data != 0x70) {
        send_log(VERBOSITY_ERROR, "FIFO_EN register mismatch, got 0x%02X, expected 0x70");
        return HAL_ERROR;
    }
    HAL_Delay(1000);

    send_log(VERBOSITY_INFO, "MPU6050 FIFO configured successfully");
    is_active = true;

    // Wait for FIFO to start filling
    HAL_Delay(1000);

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
