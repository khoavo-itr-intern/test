/*
 * flash.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Admin
 */

#include "flash.h"

/* Function pointer for jumping to user application. */
typedef void (*fnc_ptr)(void);

/**
 * @brief   This function erases the memory.
 * @param   address: First address to be erased (the last is the end of the flash).
 * @return  status: Report about the success of the erasing.
 */
flash_status flash_erase(uint32_t address)
{
  HAL_FLASH_Unlock();

  flash_status status = FLASH_ERROR;
  FLASH_EraseInitTypeDef erase_init;
  uint32_t error = 0u;

  erase_init.TypeErase = TYPEERASE_SECTORS;
  erase_init.Sector = address;
  erase_init.Banks = FLASH_BANK_1;
  /* Calculate the number of pages from "address" and the end of flash. */
  erase_init.NbSectors = (FLASH_BANK_1 - address) / FLASH_CR_PSIZE;
  /* Do the actual erasing. */
  if (HAL_OK == HAL_FLASHEx_Erase(&erase_init, &error))
  {
    status = FLASH_OK;
  }

  HAL_FLASH_Lock();

  return status;
}

/**
 * @brief   This function flashes the memory.
 * @param   address: First address to be written to.
 * @param   *data:   Array of the data that we want to write.
 * @param   *length: Size of the array.
 * @return  status: Report about the success of the writing.
 */
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length)
{
  flash_status status = FLASH_OK;

  HAL_FLASH_Unlock();

  /* Loop through the array. */
  for (uint32_t i = 0u; (i < length) && (FLASH_OK == status); i++)
  {
    /* If we reached the end of the memory, then report an error and don't do anything else.*/
    if (FLASH_APP_END_ADDRESS <= address)
    {
      status |= FLASH_ERROR_SIZE;
    }
    else
    {
      /* The actual flashing. If there is an error, then report it. */
      if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]))
      {
        status |= FLASH_ERROR_WRITE;
      }
      /* Read back the content of the memory. If it is wrong, then report an error. */
      if (((data[i])) != (*(volatile uint32_t*)address))
      {
        status |= FLASH_ERROR_READBACK;
      }

      /* Shift the address by a word. */
      address += 4u;
    }
  }

  HAL_FLASH_Lock();

  return status;
}

/**
 * @brief   Actually jumps to the user application.
 * @param   void
 * @return  void
 */
void flash_jump_to_app(void)
{
  /* Function pointer to the address of the user application. */
  fnc_ptr jump_to_app;

  /* Get the address of the function pointer from the first 4 bytes of the application area. */
  jump_to_app = (fnc_ptr)(*(volatile uint32_t*) (FLASH_APP_START_ADDRESS+4u));

  /* Deinitialize HAL to reset hardware configurations to init new app */
  HAL_DeInit();

  /* Set the Main Stack Pointer (MSP) to the value stored in the application's start address. */
  __set_MSP(*(volatile uint32_t*)FLASH_APP_START_ADDRESS);

  /* Jump to the application code using the function pointer. */
  jump_to_app();
}
