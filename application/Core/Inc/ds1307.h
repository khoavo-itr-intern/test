/**
 * @file       ds1307.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.1
 * @date       2023-08-20
 * @author     Thao Ton
 *
 * @brief      <A brief description of the content of the file>
 *
 */
/* Define to prevent recursive inclusion ------------------------------ */
#ifndef INC_ds1307_H_
#define INC_ds1307_H_

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include <stdbool.h>
extern I2C_HandleTypeDef hi2c1;

/* Public defines ----------------------------------------------------- */
#define DS1307_I2C_ADDR      0x68
#define DS1307_REG_SECOND    0x00
#define DS1307_REG_MINUTE    0x01
#define DS1307_REG_HOUR      0x02
#define DS1307_REG_DOW       0x03
#define DS1307_REG_DATE      0x04
#define DS1307_REG_MONTH     0x05
#define DS1307_REG_YEAR      0x06
#define DS1307_REG_CONTROL   0x07
#define DS1307_REG_UTC_HR    0x08
#define DS1307_REG_UTC_MIN   0x09
#define DS1307_REG_CENT      0x10
#define DS1307_TIMEOUT       1000

/**
 * @brief Structure to hold time and date information from DS1307 RTC.
 */
typedef struct {
    uint8_t sec;         /**< Seconds (0-59) */
    uint8_t min;         /**< Minutes (0-59) */
    uint8_t hour;        /**< Hours (0-23 in 24-hour format) */
    uint8_t dayofweek;   /**< Day of the week (0-6, Sunday-Saturday) */
    uint8_t date;        /**< Day of the month (1-31) */
    uint8_t month;       /**< Month (1-12) */
    uint16_t year;       /**< Year (2000-2099) */
} ds1307_struct;

typedef enum {
    DS1307_SUCCESS,
    DS1307_INVALID_PARAM,
    DS1307_OTHER_ERROR
} ds1307_status;

/* Public variables --------------------------------------------------- */
/**
 * @brief Decode a binary-coded decimal (BCD) value to its corresponding binary value.
 *
 * @param[in] bin BCD value to be decoded.
 * @return Decoded binary value.
 */
uint8_t ds1307_decode_bcd(uint8_t bin);

/**
 * @brief Encode a decimal value to its corresponding binary-coded decimal (BCD) value.
 *
 * @param[in] dec Decimal value to be encoded.
 * @return Encoded BCD value.
 */
uint8_t ds1307_encode_bcd(uint8_t dec);

/**
 * @brief Get the clock halt flag from the DS1307 RTC.
 *
 * @return Clock halt flag (1 if halted, 0 if not halted).
 */
uint8_t ds1307_get_clock_halt(void);

/**
 * @brief Read a byte from a specific register of the DS1307 RTC.
 *
 * @param[in] reg_addr Register address to read from.
 * @return Read byte value.
 */
uint8_t ds1307_get_reg_byte(uint8_t reg_addr);

bool ds1307_is_configured(void);

/* Public function prototypes ---------------------------------------- */
/**
 * @brief Set the clock halt flag of the DS1307 RTC.
 *
 * @param[in] halt Halt flag value (1 to halt, 0 to resume).
 * @return None.
 */
void ds1307_set_clock_halt(uint8_t halt);

/**
 * @brief Write a byte to a specific register of the DS1307 RTC.
 *
 * @param[in] reg_addr Register address to write to.
 * @param[in] val Byte value to be written.
 * @return None.
 */
void ds1307_set_reg_byte(uint8_t reg_addr, uint8_t val);

/**
 * @brief Set the time zone offset in hours and minutes for the DS1307 RTC.
 *
 * @param[in] hr Time zone offset in hours.
 * @param[in] min Time zone offset in minutes.
 * @return None.
 */
void ds1307_set_time_zone(int8_t hr, uint8_t min);

/**
 * @brief Configure the DS1307 RTC with default settings.
 *
 * This function initializes the DS1307 RTC by disabling the clock halt flag and setting the time zone to a default value.
 *
 * @return None.
 */
void ds1307_config(void);

/**
 * @brief Read the current time and date from the DS1307 RTC.
 *
 * @param[out] ds1307 Pointer to a ds1307_struct structure to store the read time and date.
 * @return None.
 */
void ds1307_get_time(ds1307_struct *ds1307);

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
void ds1307_set_time(ds1307_struct *ds1307, uint8_t sec, uint8_t min, uint8_t hour_24mode, uint8_t dayofweek, uint8_t date, uint8_t month, uint16_t year);

void handle_ds1307_result(uint8_t result);
#endif /* INC_ds1307_THAO_H_ */
/* End of file -------------------------------------------------------- */


