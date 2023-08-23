/**
 * @file       ds1307.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.1
 * @date       2023-08-20
 * @author     Thao Ton
 *
 * @brief      Implementation of functions related to DS1307 RTC.
 *
 */

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include "ds1307.h"

/* Public variables --------------------------------------------------- */
static bool ds1307_configured = false;
static bool ds1307_time_is_set = false;

bool ds1307_is_configured(void) {
	return ds1307_configured;
}
I2C_HandleTypeDef hi2c1;
/**
 * @brief Read a byte from a specific register of the DS1307 RTC.
 *
 * @param[in] reg_addr Register address to read from.
 * @return Read value.
 */
uint8_t ds1307_get_reg_byte(uint8_t reg_addr)
{
	uint8_t val;
	HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDR << 1, &reg_addr, 1, DS1307_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, DS1307_I2C_ADDR << 1, &val, 1, DS1307_TIMEOUT);
	return val;
}

/**
 * @brief Get the clock halt flag from the DS1307 RTC.
 *
 * @return Clock halt flag (1 if halted, 0 if running).
 */
uint8_t ds1307_get_clock_halt(void)
{
	return (ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x80) >> 7;
}

/**
 * @brief Decode a binary-coded decimal (BCD) value to decimal.
 *
 * @param[in] bin Binary-coded decimal value.
 * @return Decoded decimal value.
 */
uint8_t ds1307_decode_bcd(uint8_t bin)
{
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encode a decimal value to binary-coded decimal (BCD).
 *
 * @param[in] dec Decimal value.
 * @return Encoded BCD value.
 */
uint8_t ds1307_encode_bcd(uint8_t dec)
{
	return (dec % 10 + ((dec / 10) << 4));
}

/* Public function prototypes ---------------------------------------- */
/**
 * @brief Get the current time and date from the DS1307 RTC.
 *
 * @param[out] ds1307 Pointer to a ds1307_struct structure to store the read time and date.
 * @return None.
 */
void ds1307_get_time(ds1307_struct *ds1307)
{
	// Check if time has been set
	if (!ds1307_time_is_set)
	{
		// Time has not been set, handle this case...
		return;
	}

	uint16_t cen;
	cen = ds1307_get_reg_byte(DS1307_REG_CENT) * 100;
	ds1307->sec = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x7f);
	ds1307->min = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_MINUTE));
	ds1307->hour = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_HOUR) & 0x3f);
	ds1307->dayofweek = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_DOW));
	ds1307->date = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_DATE));
	ds1307->month = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_MONTH));
	ds1307->year = ds1307_decode_bcd(ds1307_get_reg_byte(DS1307_REG_YEAR)) + cen;
}

/**
 * @brief Set the value of a specific register in the DS1307 RTC.
 *
 * @param[in] reg_addr Register address to write to.
 * @param[in] val Value to be written.
 * @return None.
 */
void ds1307_set_reg_byte(uint8_t reg_addr, uint8_t val)
{
	uint8_t bytes[2] = {reg_addr, val};
	HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDR << 1, bytes, 2, DS1307_TIMEOUT);
}

/**
 * @brief Set the time and date in the DS1307 RTC.
 *
 * @param[in,out] ds1307 Pointer to a ds1307_struct structure containing the new time and date values.
 * @param[in] sec Seconds value (0 to 59).
 * @param[in] min Minutes value (0 to 59).
 * @param[in] hour_24mode Hour value in 24-hour format (0 to 23).
 * @param[in] dayofweek Day of the week (0 to 6, where 0 represents Sunday).
 * @param[in] date Day of the month (1 to 31).
 * @param[in] month Month value (1 to 12).
 * @param[in] year Year value (2000 to 2099).
 * @return None.
 */

void ds1307_set_time(ds1307_struct *ds1307, uint8_t sec, uint8_t min, uint8_t hour_24mode, uint8_t dayofweek, uint8_t date, uint8_t month, uint16_t year)
{
	if (sec >= 60 || sec < 0 || min >= 60 || min < 0 || hour_24mode >= 24 || hour_24mode < 0 || dayofweek > 6 || dayofweek < 0 || date < 1 || date > 31 || month < 1 || month > 12 || year < 2000 || year > 2099)
	{
		// Handle this case
		return;
	}
	ds1307_time_is_set = 1;

	ds1307_set_reg_byte(DS1307_REG_SECOND, ds1307_encode_bcd(sec | ds1307_get_clock_halt()));
	ds1307_set_reg_byte(DS1307_REG_MINUTE, ds1307_encode_bcd(min));
	ds1307_set_reg_byte(DS1307_REG_HOUR, ds1307_encode_bcd(hour_24mode & 0x3f));
	ds1307_set_reg_byte(DS1307_REG_DOW, ds1307_encode_bcd(dayofweek));
	ds1307_set_reg_byte(DS1307_REG_DATE, ds1307_encode_bcd(date));
	ds1307_set_reg_byte(DS1307_REG_MONTH, ds1307_encode_bcd(month));
	ds1307_set_reg_byte(DS1307_REG_CENT, year / 100);
	ds1307_set_reg_byte(DS1307_REG_YEAR, ds1307_encode_bcd(year % 100));
}

/**
 * @brief Set the clock halt flag of the DS1307 RTC.
 *
 * @param[in] halt Halt flag value (1 to halt, 0 to resume).
 * @return None.
 */
void ds1307_set_clock_halt(uint8_t halt)
{
	uint8_t ch = (halt ? 1 << 7 : 0);
	ds1307_set_reg_byte(DS1307_REG_SECOND, ch | (ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x7f));
}

/**
 * @brief Set the time zone offset in hours and minutes for the DS1307 RTC.
 *
 * @param[in] hr Time zone offset in hours.
 * @param[in] min Time zone offset in minutes.
 * @return None.
 */
void ds1307_set_time_zone(int8_t hr, uint8_t min)
{
	ds1307_set_reg_byte(DS1307_REG_UTC_HR, hr);
	ds1307_set_reg_byte(DS1307_REG_UTC_MIN, min);
}

/**
 * @brief Configure the DS1307 RTC with default settings.
 *
 * This function initializes the DS1307 RTC by disabling the clock halt flag and setting the time zone to a default value.
 *
 * @return None.
 */
void ds1307_config(void)
{
	ds1307_set_clock_halt(0);
	ds1307_set_time_zone(+7, 00);
	ds1307_configured = true;
}

/* End of file -------------------------------------------------------- */
