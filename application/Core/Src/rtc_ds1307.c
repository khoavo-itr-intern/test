/*
 * rtc_ds1307.c
 *
 *  Created on: Aug 14, 2023
 *      Author: Admin
 */

#include "rtc_ds1307.h"
#include "main.h"

I2C_HandleTypeDef *_ds1307_ui2c;

typedef struct
{
	uint8_t seconds; // Seconds, from 00 to 59
	uint8_t minutes; // Minutes, from 00 to 59
	uint8_t hours;	 // Hours, 24Hour mode, 00 to 23
	uint8_t dow;	 // Day in a week, from 1 to 7
	uint8_t date;	 // Date in a month, 1 to 31
	uint8_t month;	 // Month in a year, 1 to 12
	uint16_t year;	 // Year, 00 to 99, 00 is 2000 and 99 is 2099
} ds1307_time_t;
ds1307_time_t ds1307;

/*-----------------------------------------------Init-------------------------------------------------------*/

/**
 * @brief Initializes the DS1307 module. Sets clock halt bit to 0 to start timing.
 * @param hi2c User I2C handle pointer.
 */
void ds1307_init(I2C_HandleTypeDef *hi2c)
{
  _ds1307_ui2c = hi2c;
  ds1307_set_clock_halt(0);

  //Check if device is connected
  // if (...)
  //   return TM_DS1307_Result_DeviceNotConnected;
  // else
  //   return TM_DS1307_Result_Ok;
}

/**
 * @brief Sets clock halt bit.
 * @param halt Clock halt bit to set, 0 or 1. 0 to start timing, 1 to stop.
 */
void ds1307_set_clock_halt(uint8_t halt)
{
  uint8_t clock_halt = (halt ? 1 << 7 : 0);
  ds1307_set_reg_byte(DS1307_REG_SECOND, clock_halt | (ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x7f));
}

/**
 * @brief Gets clock halt bit.
 * @return Clock halt bit, 0 or 1.
 */
uint8_t ds1307_get_clock_halt(void)
{
  return (ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x80) >> 7;
}

/*-----------------------------------------------Get - Set Register-------------------------------------------------------*/

/**
 * @brief Sets the byte in the designated DS1307 register to value.
 * @param regAddr Register address to write.
 * @param val Value to set, 0 to 255.
 */
void ds1307_set_reg_byte(uint8_t regAddr, uint8_t val)
{
  uint8_t bytes[2] = {regAddr, val};
  HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, bytes, 2, DS1307_TIMEOUT);
}

/**
 * @brief Gets the byte in the designated DS1307 register.
 * @param regAddr Register address to read.
 * @return Value stored in the register, 0 to 255.
 */
uint8_t ds1307_get_reg_byte(uint8_t regAddr)
{
  uint8_t val;
  HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &regAddr, 1, DS1307_TIMEOUT);
  HAL_I2C_Master_Receive(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &val, 1, DS1307_TIMEOUT);
  return val;
}

/*-----------------------------------------------SQW/OUT pin-------------------------------------------------------*/

/** Enable SQW/OUT pin 
 *  It can output 32768Hz, 8192Hz, 4096Hz, 1Hz, Low or High state
 *  Need external pull up resistor (or pull up from MCU)
 */
void ds1307_enable_output_pin(ds1307_output_frequency_t frequency)
{
    uint8_t temp;
    if (frequency == DS1307_SQW_1HZ) 
    {
        temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE;
    } 
    else if (frequency == DS1307_SQW_4096HZ) 
    {
        temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS0;
    } 
    else if (frequency == DS1307_SQW_8192HZ) 
    {
        temp =  1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1;
    } 
    else if (frequency == DS1307_SQW_32768HZ) 
    {
        temp = 1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1 | 1 << DS1307_CONTROL_RS0;
    } 
    else if (frequency == DS1307_OUT_HIGH) 
    {
        temp = 1 << DS1307_CONTROL_OUT;
    } 
    else if (frequency == DS1307_OUT_LOW) 
    {
        temp = 0;
    }
    // Write to register
    ds1307_set_reg_byte(DS1307_REG_CONTROL, temp);
}

/** Disable SQW/OUT pin 
 *  here choose default is HIGH
 */
void ds1307_disable_output_pin(void)
{
    /* Set output pin to high */
    ds1307_enable_output_pin(DS1307_OUT_HIGH);
}

/*-----------------------------------------------Get Time-------------------------------------------------------*/

/**
 * Get full time
*/
void ds1307_get_date_time()
{
    uint16_t cen;
    ds1307.seconds=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x7f);
    ds1307.minutes=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_MINUTE));
    ds1307.hours=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_HOUR) & 0x3f);
    ds1307.dow=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_DOW));
    ds1307.date=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_DATE));
    ds1307.month=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_MONTH));
    cen = ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_CENT));
    ds1307.year=ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_YEAR)) + (cen * 100);
}

/**
 * @brief Gets the current day of week.
 * @return Days from last Sunday, 0 to 6.
 */
uint8_t ds1307_get_dayofweek(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_DOW));
}

/**
 * @brief Gets the current day of month.
 * @return Day of month, 1 to 31.
 */
uint8_t ds1307_get_date(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_DATE));
}

/**
 * @brief Gets the current month.
 * @return Month, 1 to 12.
 */
uint8_t ds1307_get_month(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_MONTH));
}

/**
 * @brief Gets the current year.
 * @return Year, 2000 to 2099.
 */
uint16_t ds1307_get_year(void)
{
  uint16_t cen = ds1307_get_reg_byte(DS1307_REG_CENT) * 100;
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_YEAR)) + cen;
}

/**
 * @brief Gets the current hour in 24h format.
 * @return Hour in 24h format, 0 to 23.
 */
uint8_t ds1307_get_hour(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_HOUR) & 0x3f);
}

/**
 * @brief Gets the current minute.
 * @return Minute, 0 to 59.
 */
uint8_t ds1307_get_minute(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_MINUTE));
}

/**
 * @brief Gets the current second. Clock halt bit not included.
 * @return Second, 0 to 59.
 */
uint8_t ds1307_get_second(void)
{
  return ds1307_bin_to_bcd(ds1307_get_reg_byte(DS1307_REG_SECOND) & 0x7f);
}

/*----------------------------------------------Set Time-------------------------------------------------------*/

/**
 * set time zone (07:00)
 */ 
void ds1307_set_time_zone(int8_t hour, uint8_t min)
{
    ds1307_set_reg_byte(DS1307_REG_UTC_HR, hour);
    ds1307_set_reg_byte(DS1307_REG_UTC_MIN, min);
}

/**
 * Set full time
*/
void ds1307_set_date_time(uint8_t second, uint8_t minute, uint8_t hour_24mode, uint8_t dayofweek, uint8_t date, uint8_t month, uint16_t year)
{
    ds1307_set_reg_byte(DS1307_REG_SECOND, ds1307_bcd_to_bin(second | ds1307_get_clock_halt())); //second Second, 0 to 59
    ds1307_set_reg_byte(DS1307_REG_MINUTE, ds1307_bcd_to_bin(minute));  //minute Minute, 0 to 59
    ds1307_set_reg_byte(DS1307_REG_HOUR, ds1307_bcd_to_bin(hour_24mode & 0x3f));//hour_24mode Hour in 24h format, 0 to 23.
    ds1307_set_reg_byte(DS1307_REG_DOW, ds1307_bcd_to_bin(dayofweek));  //dayOfWeek - Days since last Sunday, 1 to 7.
    ds1307_set_reg_byte(DS1307_REG_DATE, ds1307_bcd_to_bin(date));      //date - Day of month, 1 to 31.
    ds1307_set_reg_byte(DS1307_REG_MONTH, ds1307_bcd_to_bin(month));    //month - Month, 1 to 12.
    ds1307_set_reg_byte(DS1307_REG_CENT, ds1307_bcd_to_bin(year / 100));                   //year - Year, 2000 - 2099
    ds1307_set_reg_byte(DS1307_REG_YEAR, ds1307_bcd_to_bin(year % 100));// ex: 2023 --> CENT=20 and YEAR=23
}

/**
 * @brief Sets the current day of week.
 * @param dayOfWeek Days since last Sunday, 0 to 6.
 */
void ds1307_set_dayofweek(uint8_t dayofweek)
{
  ds1307_set_reg_byte(DS1307_REG_DOW, ds1307_bcd_to_bin(dayofweek));
}

/**
 * @brief Sets the current day of month.
 * @param date Day of month, 1 to 31.
 */
void ds1307_set_date(uint8_t date)
{
  ds1307_set_reg_byte(DS1307_REG_DATE, ds1307_bcd_to_bin(date));
}

/**
 * @brief Sets the current month.
 * @param month Month, 1 to 12.
 */
void ds1307_set_month(uint8_t month)
{
  ds1307_set_reg_byte(DS1307_REG_MONTH, ds1307_bcd_to_bin(month));
}

/**
 * @brief Sets the current year.
 * @param year Year, 2000 to 2099.
 */
void ds1307_set_year(uint16_t year)
{
  ds1307_set_reg_byte(DS1307_REG_CENT, year / 100);
  ds1307_set_reg_byte(DS1307_REG_YEAR, ds1307_bcd_to_bin(year % 100));
}

/**
 * @brief Sets the current hour, in 24h format.
 * @param hour_24mode Hour in 24h format, 0 to 23.
 */
void ds1307_set_hour(uint8_t hour_24mode)
{
  ds1307_set_reg_byte(DS1307_REG_HOUR, ds1307_bcd_to_bin(hour_24mode & 0x3f));
}

/**
 * @brief Sets the current minute.
 * @param minute Minute, 0 to 59.
 */
void ds1307_set_minute(uint8_t minute)
{
  ds1307_set_reg_byte(DS1307_REG_MINUTE, ds1307_bcd_to_bin(minute));
}

/**
 * @brief Sets the current second.
 * @param second Second, 0 to 59.
 */
void ds1307_set_second(uint8_t second)
{
  uint8_t clock_halt = ds1307_get_clock_halt();
  ds1307_set_reg_byte(DS1307_REG_SECOND, ds1307_bcd_to_bin(second | clock_halt));
}

/*-----------------------------------------------Convert BCD - BIN-------------------------------------------------------*/

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
uint8_t ds1307_bin_to_bcd(uint8_t bin)
{
  return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param bcd BCD number to encode.
 * @return Encoded binary-coded decimal value.
 */
uint8_t ds1307_bcd_to_bin(uint8_t bcd)
{
  return (bcd % 10 + ((bcd / 10) << 4));
}




