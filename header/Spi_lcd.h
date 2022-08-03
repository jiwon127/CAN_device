/*
 * Spi_lcd.h
 *
 *  Created on: 2021. 2. 15.
 *      Author: kjm-hsh
 */

#ifndef HEADER_SPI_LCD_H_
#define HEADER_SPI_LCD_H_

void spi_init(void);
void spi_conf(void);
void spi_init_conf(void);

void spi_cs_enable(void);
void spi_cs_disable(void);

void spi_send_byte(short data);
void spi_send_str(char *buffer);
short spi_receive_byte(void);

#endif /* HEADER_SPI_LCD_H_ */
