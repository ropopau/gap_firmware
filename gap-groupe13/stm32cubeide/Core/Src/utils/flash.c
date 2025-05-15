/**
  ******************************************************************************
  * @file    flash.c
  * @author  Group 13
  * @brief   This file manage flash manipulation.
  *
  *
  ******************************************************************************
  */

#include "utils/flash.h"

/**
  * @brief  Erase a given flash sector.
  *
  * @param  uint32_t sector	Sector to rease
  * @retval true if success, false else.
  */
bool flash_erase_sector(uint32_t sector) {

    HAL_FLASH_Unlock();

    // Wiping sector 7 (last one in the nucleo board)
    FLASH_EraseInitTypeDef erase_config = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = sector,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };

    uint32_t sector_error;
    if (HAL_FLASHEx_Erase(&erase_config, &sector_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }
    HAL_FLASH_Lock();

    return true;
}

/**
  * @brief  Write data
  *
  * @param  uint32_t *address	address where the data should be written.
  * @param  const void *data	data to write.
  * @param  uint32_t size	size of the data to write.
  *
  * @retval true if success, false else.
  */
bool flash_write_data(uint32_t *address, const void *data, uint32_t size) {
	HAL_FLASH_Unlock();

    // Align the buffer to 32 bits
	const uint32_t *src = (const uint32_t*)data;
	uint32_t *dest = address;


	// Writing byte by byte into the reserved flash
	 for (uint8_t i = 0; i < size / 4; i++) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)dest++, *src++) != HAL_OK) {
			HAL_FLASH_Lock();
			return false;
		}
	}

	HAL_FLASH_Lock();

    return true;
}

/**
  * @brief  read data
  *
  * @param  uint32_t address	address where the data to read is stored.
  * @param  void *data	Data buffer: the read data is stored at this address.
  * @param  uint32_t size	size of the data to read.
  *
  * @retval always true.
  */
bool flash_read_data(uint32_t address, void *data, uint32_t size) {
    memcpy(data, (void*)address, size);
    return true;
}
