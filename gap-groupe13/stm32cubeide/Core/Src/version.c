/**
  ******************************************************************************
  * @file    version.c
  * @author  Group 13
  * @brief   Version managing in a persistent flash storage.
  *
  ******************************************************************************
  */

#include "version.h"
#include "stm32f4xx_hal.h"

// Getting address where version is stored.
// (You can look at the *FLASH.ld file to look at this flash storage defining)
__attribute__((section(".version")))
static volatile uint8_t _version_data[VERSION_SIZE];

// Default version is none is defined.
static const char *default_version = "0.0.0";


/**
  * @brief  Check if the version follow the SemVer's rules
  * 		For now, the version accept only the Major.Minor.Patch
  * 		TODO: Optional metadatas handling
  *
  * @param  const char* version		version to check.
  * @retval None
  */
bool is_valid_version(const char* version) {

	if (strlen(version) > VERSION_SIZE) {
		send_log(VERBOSITY_ERROR, "Given version is too long. Should be maximum 15 characters.");
		return false;
	}

	const unsigned char *ptr = (const unsigned char *)version;


	for (int i = 0; i < 3; i++) {
		if (!isdigit(*ptr)) return false;

		// Checking that a zero is not used as a padding
		if (*ptr == '0' && isdigit(*(ptr + 1))) return false;

		while (isdigit(*ptr)) ptr++;
		if (i < 2 && *ptr != '.') return false;
		ptr++;
	}

	// TODO Optional metadata

	return *ptr == '\0';

}

/**
  * @brief  Get the stored version. Set using default_version is not defined.
  *
  * @param  char* version_buffer		buffer where the read version should be stored.
  * @retval None
  */
void get_version(char *version_buffer) {

	// Checking that nothing is written in the version addresses
    if (_version_data[0] == 0xFF || _version_data[0] == 0x00) {
    	send_log(VERBOSITY_INFO, "No version is defined yet. Defining to 0.0.0");
		set_version(default_version);
	}

    for (int i = 0; i < VERSION_SIZE; i++) {
    	version_buffer[i] = _version_data[i];
	}

    return;
}

/**
  * @brief  Set a new version.
  *  - Erase the last sector (where the version flash address is)
  *  - Write the new version at the version address.
  *
  *  Data should be align with 32.
  *
  * @param  	const char *new_version		New version to set.
  * @retval		always true
  */
bool set_version(const char *new_version) {
	flash_erase_sector(FLASH_SECTOR_7);
    flash_write_data((uint32_t *)_version_data, new_version, VERSION_SIZE);
    return true;
}
