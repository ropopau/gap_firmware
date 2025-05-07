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




bool _is_valid_version(const char* version) {

	if (strlen(version) > 15) {
		send_log(VERBOSITY_ERROR, "Given version is too long. Should be maximum 15 characters.");
		return false;
	}


	const char *ptr = version;


	for (int i = 0; i < 3; i++) {
		if (!isdigit(*ptr)) return false;

		// Si un 0 est utilise comme padding dans le numero de version
		if (*ptr == '0' && isdigit(*(ptr + 1))) return false;

		while (isdigit(*ptr)) ptr++;
		if (i < 2 && *ptr != '.') return false;
		ptr++;
	}

	// TODO pour metadonnee optionnelles

	/*
	if (*ptr == '-') {
		ptr++;
		// Si le premier element n'est ni alpha num, ni '-' ni '.'
		if (!isalnum(*ptr) && *ptr != '-' && *ptr != '.') return false;


		// Tant que ca correspond au regex on incremente le pointer.
		while (isalnum(*ptr) || *ptr == '-' || *ptr == '.') ptr++;
	}

	if (*ptr == '+') {
		ptr++;
		// Si le premier element n'est ni alpha num, ni '-' ni '.'
		if (!isalnum(*ptr) && *ptr != '-' && *ptr != '.') return false;

		// Tant que ca correspond au regex on incremente le pointer.
		while (isalnum(*ptr) || *ptr == '-' || *ptr == '.') ptr++;
	}
	*/
	return *ptr == '\0';

}




void get_version(uint8_t *buffer) {
    if (!buffer) return;

    if (_version_data[0] == 0xFF || _version_data[0] == 0x00) {
		set_version("0.0.0");
	}

    memcpy(buffer, _version_data, VERSION_SIZE);
    return;
}

void set_version(const uint8_t *new_version) {
    if (!new_version) return;

    if (!_is_valid_version(new_version)){
    	send_log(VERBOSITY_ERROR, "Given version does not follow Semantic Versioning rules: %s", new_version);
    	return;
    }


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
    const uint32_t *src = (const uint32_t*)new_version;
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
