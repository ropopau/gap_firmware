/*
 * version.c
 *
 *  Created on: May 5, 2025
 *      Author: sanghyeon
 */

#include "version.h"
#include "stm32f4xx_hal.h"

__attribute__((section(".version")))
static volatile uint8_t _version_data[VERSION_SIZE];

void get_version(uint8_t *buffer) {
    if (!buffer) return;

    uint8_t *toto = _version_data;
    if (_version_data[0] == 0xFF || _version_data[0] == 0x00) {
		set_version("0.0.0");
	}

    memcpy(buffer, _version_data, VERSION_SIZE);
    return;
}

void set_version(const uint8_t *buffer) {
    if (!buffer) return;

    HAL_FLASH_Unlock();

    // On wipe le secteur 7 (le dernier pour la nucleo)
    FLASH_EraseInitTypeDef erase_config = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = FLASH_SECTOR_7,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };

    uint32_t sector_error;
    if (HAL_FLASHEx_Erase(&erase_config, &sector_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }

    // On aligne le buffer source sur 32bit
    const uint32_t *src = (const uint32_t*)buffer;
    uint32_t *dest = (uint32_t*)_version_data;


    // On ecrit byte par byte
    for (uint8_t i = 0; i < VERSION_SIZE / 4; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)dest++, *src++) != HAL_OK) {
            HAL_FLASH_Lock();
            return;
        }
    }

    HAL_FLASH_Lock();
    return;
}
