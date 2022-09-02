/*
 * FT81X_LCD.c
 *
 *  Created on: 2021. 2. 24.
 *      Author: kjm-hsh
 */

#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/eve2_lib.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

uint16_t FifoWriteLocation = 0;

void SPI_LCD_ACTIVE(void)
{

    spi_cs_enable();

    spi_send_byte(0x00);
    spi_receive_byte();
    spi_send_byte(0x00);
    spi_receive_byte();
    spi_send_byte(0x00);
    spi_receive_byte();
    delay_ms(500);

    spi_cs_disable();

}

void SPI_LCD_CLKEXT(void)
{
    spi_cs_enable();

    spi_send_byte(0x44);
    spi_receive_byte();
    spi_send_byte(0x00);
    spi_receive_byte();
    spi_send_byte(0x00);
    spi_receive_byte();

    spi_cs_disable();

}

uint8_t spi_read_8(int address)
{
    volatile short garbage = 0;
    volatile short realData = 0;

    spi_cs_enable();

    spi_send_byte( ( address >> 16) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( ( address >> 8) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( address & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte(0x0);
    garbage = spi_receive_byte();
    //dummy

    spi_send_byte(0x0);
    realData = spi_receive_byte();

    spi_cs_disable();

    return realData;
}

uint16_t spi_read_16(int address)
{
    volatile short garbage = 0;
    volatile short realData_array[2] = {0,};

    spi_cs_enable();

    spi_send_byte( ( address >> 16) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( ( address >> 8) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( address & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte(0x0);
    garbage = spi_receive_byte();
    //dummy

    spi_send_byte(0x0);
    realData_array[0] = spi_receive_byte();
    spi_send_byte(0x0);
    realData_array[1] = spi_receive_byte();


    spi_cs_disable();

    uint16_t realData= realData_array[0]+ (realData_array[1]<<8);
    return realData;
}

uint32_t spi_read_32(int address)
{
    volatile short garbage = 0;
    volatile short realData_array[4] = {0,};

    spi_cs_enable();

    spi_send_byte( ( address >> 16) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( ( address >> 8) & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte( address & 0xFF);
    garbage = spi_receive_byte();
    spi_send_byte(0x0);
    garbage = spi_receive_byte();
    //dummy

    spi_send_byte(0x0);
    realData_array[0] = spi_receive_byte();
    spi_send_byte(0x0);
    realData_array[1] = spi_receive_byte();
    spi_send_byte(0x0);
    realData_array[2] = spi_receive_byte();
    spi_send_byte(0x0);
    realData_array[3] = spi_receive_byte();


    spi_cs_disable();

    uint32_t realData= realData_array[0]+ (realData_array[1]<<8)
                    +(realData_array[2]<<16)+ (realData_array[3]<<24);
    return realData;
}



void spi_write_8(int address,uint8_t data)
{
    volatile short garbage = 0;
    volatile short realData = 0;

    spi_cs_enable();

    spi_send_byte( ( address >> 16) | 0x80);
    garbage = spi_receive_byte();
    spi_send_byte(  address >> 8 );
    garbage = spi_receive_byte();
    spi_send_byte( address);
    garbage = spi_receive_byte();


    spi_send_byte(data);
    garbage = spi_receive_byte();

    spi_cs_disable();
}

void spi_write_16(int address,uint16_t data)
{
    volatile short garbage = 0;
    volatile short realData_array[2] = {0,};

    spi_cs_enable();

    spi_send_byte(( address >> 16) | 0x80);
    garbage = spi_receive_byte();
    spi_send_byte( address >> 8 );
    garbage = spi_receive_byte();
    spi_send_byte( address);
    garbage = spi_receive_byte();

    spi_send_byte(data & 0xFF);
    garbage = spi_receive_byte();

    spi_send_byte(data >> 8);
    garbage = spi_receive_byte();


    spi_cs_disable();
}


void spi_write_32(int address,uint32_t data)
{
    volatile short garbage = 0;
    volatile short realData_array[4] = {0,};

    spi_cs_enable();

    spi_send_byte( ( address >> 16) | 0x80);
    garbage = spi_receive_byte();
    spi_send_byte(  address >> 8 );
    garbage = spi_receive_byte();
    spi_send_byte( address);
    garbage = spi_receive_byte();


    spi_send_byte(data & 0xFF );
    garbage = spi_receive_byte();

    spi_send_byte((data >> 8) & 0xFF );
    garbage = spi_receive_byte();

    spi_send_byte((data >> 16) & 0xFF );
    garbage = spi_receive_byte();

    spi_send_byte((data >> 24) & 0xFF );
    garbage = spi_receive_byte();


    spi_cs_disable();
}


