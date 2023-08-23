/*
 * rtc_ds1307.h
 *
 *  Created on: Aug 14, 2023
 *      Author: Admin
 */

#ifndef INC_RTC_DS1307_H_
#define INC_RTC_DS1307_H_

#include "main.h"

extern I2C_HandleTypeDef *_ds1307_ui2c;

/* DS1307 I2C clock */
#define DS1307_I2C_CLOCK    100000

/* I2C slave address for DS1307 */
#define DS1307_I2C_ADDR 	0x68

/* Registers location */
#define DS1307_REG_SECOND 	0x00
#define DS1307_REG_MINUTE 	0x01
#define DS1307_REG_HOUR 	0x02
#define DS1307_REG_DOW 		0x03
#define DS1307_REG_DATE		0x04
#define DS1307_REG_MONTH 	0x05
#define DS1307_REG_YEAR 	0x06
#define DS1307_REG_CONTROL 	0x07
#define DS1307_REG_UTC_HR   0x08
#define DS1307_REG_UTC_MIN  0x09
#define DS1307_REG_CENT     0x10
#define DS1307_TIMEOUT 		1000

/* Bits in control register */
#define DS1307_CONTROL_OUT 	7 // use to translate bit 1
#define DS1307_CONTROL_SQWE 4
#define DS1307_CONTROL_RS1 	1
#define DS1307_CONTROL_RS0 	0

/**
 * @brief  Result state
 */
typedef enum
{
	TM_DS1307_Result_Ok = 0,			// ok
	TM_DS1307_Result_Error,				// error
	TM_DS1307_Result_DeviceNotConnected // not connected
} ds1307_result_t;

/**
 * @brief  SQW/OUT pin
 */
typedef enum
{
	DS1307_SQW_1HZ = 0, // set sqw/out pin at sqw mode, with frequence  1	 Hz
	DS1307_SQW_4096HZ,	//											   4096	 Hz
	DS1307_SQW_8192HZ,	//											   8192	 Hz
	DS1307_SQW_32768HZ, //											   32768 Hz
	DS1307_OUT_HIGH,	// set sqw/out pin at out mode - HIGH
	DS1307_OUT_LOW		// set sqw/out pin at out mode - LOW
} ds1307_output_frequency_t;

/**
 * @brief  Structure for date and time
 */
//typedef struct
//{
//	uint8_t seconds; // Seconds, from 00 to 59
//	uint8_t minutes; // Minutes, from 00 to 59
//	uint8_t hours;	 // Hours, 24Hour mode, 00 to 23
//	uint8_t dow;	 // Day in a week, from 1 to 7
//	uint8_t date;	 // Date in a month, 1 to 31
//	uint8_t month;	 // Month in a year, 1 to 12
//	uint8_t year;	 // Year, 00 to 99, 00 is 2000 and 99 is 2099
//} ds1307_time_t;
//ds1307_time_t ds1307;

/* Initialize the DS1307 RTC module*/
void ds1307_init(I2C_HandleTypeDef *hi2c);

/* Set or clear Clock Halt bit ((CH)) to stop (1) or continue (0) real-time clock */
void ds1307_set_clock_halt(uint8_t halt);
/* Get Clock Halt bit to case use reg second*/
uint8_t ds1307_get_clock_halt(void);

/* Set value in a specific register in DS1307 */
void ds1307_set_reg_byte(uint8_t regAddr, uint8_t val);

/* Get the value from a specific register in DS1307 */
uint8_t ds1307_get_reg_byte(uint8_t regAddr);

/** Enable SQW/OUT pin 
 *  It can output 32768Hz, 8192Hz, 4096Hz, 1Hz, Low or High state
 *  Need external pull up resistor (or pull up from MCU)
 */
void ds1307_enable_output_pin(ds1307_output_frequency_t frequency);

/** Disable SQW/OUT pin 
 */
void ds1307_disable_output_pin(void);

/* Get Time */

void ds1307_get_date_time(); 							// get full date and time

uint8_t ds1307_get_dayofweek(void); 					// get one register date or time
uint8_t ds1307_get_date(void);
uint8_t ds1307_get_month(void);
uint16_t ds1307_get_year(void);

uint8_t ds1307_get_hour(void);
uint8_t ds1307_get_minute(void);
uint8_t ds1307_get_second(void);

/* Set Time */

void ds1307_set_time_zone(int8_t hour, uint8_t min); 	// Set time zone

void ds1307_set_date_time(uint8_t second, uint8_t minute, uint8_t hour_24mode, uint8_t dayofweek, uint8_t date, uint8_t month, uint16_t year); 	// set full date and time

void ds1307_set_dayofweek(uint8_t dow);					// set one register date or time
void ds1307_set_date(uint8_t date);
void ds1307_set_month(uint8_t month);
void ds1307_set_year(uint16_t year);

void ds1307_set_hour(uint8_t hour_24mode);
void ds1307_set_minute(uint8_t minute);
void ds1307_set_second(uint8_t second);
void ds1307_set_timezone(int8_t hr, uint8_t min);

/* Convert BCD value to Bin */
uint8_t ds1307_bcd_to_bin(uint8_t bin);

/* Convert Bin value to BCD */
uint8_t ds1307_bin_to_bcd(uint8_t dec);

#endif /* INC_RTC_DS1307_H_ */
