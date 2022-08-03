/*
 * Flash_memory.h
 *
 *  Created on: 2021. 3. 26.
 *      Author: kjm-hsh
 */
#include <stdint.h>

#ifndef HEADER_FLASH_MEMORY_H_
#define HEADER_FLASH_MEMORY_H_


#define FLASH_BASE_ADDR         ((volatile uint32_t*)0x00020000)


void Flash_erase(uint32_t  start, uint32_t  end);
void Flash_erase_all();
void Flash_write(const uint32_t  *data, uint32_t  size, uint32_t  start);
void Flash_read(uint32_t * data,uint32_t  size, uint32_t  start);
void Flash_reset(uint32_t data);
void Flash_save(void);


#endif /* HEADER_FLASH_MEMORY_H_ */

