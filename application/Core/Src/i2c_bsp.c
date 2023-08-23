/**
 * @file       <i2c_bsp>.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       yyyy-mm-dd
 * @author     thiencao
 * @author     <first_name_2> <last_name_2>
 *
 * @brief      <A brief description of the content of the file>
 *
 * @note
 * @example    example_file_1.c
 *             Example_1 description
 * @example    example_file_2.c
 *             Example_2 description
 */

/* Includes ----------------------------------------------------------- */
#include "i2c_bsp.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
uint8_t bsp_i2c_transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, packet data){
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, DevAddress, data.data, data.length, HAL_MAX_DELAY);
	if (status == HAL_OK) {
		return I2C_SUCCESS;
	}
	else {
		return I2C_ERROR;
	}
}

uint8_t bsp_i2c_receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, packet data){
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(hi2c, DevAddress, data.data, data.length, HAL_MAX_DELAY);
	if (status == HAL_OK) {
		return I2C_SUCCESS;
	}
	else {
		return I2C_ERROR;
	}
}
/* End of file -------------------------------------------------------- */
