/**
 * @file       i2c_bsp.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       yyyy-mm-dd
 * @author     thiencao
 * @author     thiencao
 *
 * @brief      <A brief description of the content of the file>
 *
 * @note
 * @example    example_file_1.c
 *             Example_1 description
 * @example    example_file_2.c
 *             Example_2 description
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef INC_I2C_BSP_H_
#define INC_I2C_BSP_H_
/* Includes ----------------------------------------------------------- */
#include "stm32f4xx_hal.h"
/* Public defines ----------------------------------------------------- */
#define I2C_SUCCESS		1
#define I2C_ERROR		0

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief packet data
 */
typedef struct{
	uint8_t *data;
	uint16_t length;
}packet;
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  <bsp transmit via i2c>
 *
 * @param[in]     <hi2c>  		<I2C_HandleTypeDef>
 * @param[in]  	  <DevAddress>  <Address of i2c slave>
 * @param[in]	  <data>  		<packet data to transmit>
 *
 */
uint8_t bsp_i2c_transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, packet data);

/**
 * @brief  <bsp receive via i2c>
 *
 * @param[in]     <hi2c>  		<I2C_HandleTypeDef>
 * @param[in]     <DevAddress>  <Address of i2c slave>
 * @param[in]  	  <data> 		<packet data receive>
 *
 */
uint8_t bsp_i2c_receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, packet data);
#endif /* INC_I2C_BSP_H_ */

/* End of file -------------------------------------------------------- */
