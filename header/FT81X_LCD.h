/*
 * FT81X_LCD.h
 *
 *  Created on: 2021. 2. 24.
 *      Author: kjm-hsh
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

#ifndef HEADER_FT81X_LCD_H_
#define HEADER_FT81X_LCD_H_



void SPI_LCD_ACTIVE(void);
void SPI_LCD_CLKEXT(void);

uint8_t spi_read_8(int address);
uint16_t spi_read_16(int address);
uint32_t spi_read_32(int address);
void spi_write_8(int address,uint8_t data);
void spi_write_16(int address,uint16_t data);
void spi_write_32(int address,uint32_t data);
void spi_writebuffer( uint32_t tx_data);

#endif /* HEADER_FT81X_LCD_H_ */
