#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>
#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

int hib_flag, hib_count, timer2_flag;
volatile int speed, int_distance; // km/h
volatile double interval, distance;  //  km
char distance_array[6];

uint32_t distance_data[sizeof(distance)];

#define size 4




int main()
{
    Sysctl_Clock();
    uart_7_init();

    uart_7_tx_str("start\r\n");

    uint32_t a[size]={0,};
    uint32_t b[size]={0x30,0x31,0x32,0x33}; //must little endian
    uint32_t  i;
    uint32_t address;


    while(1)
    {
        for(i=0;i<(size/4);i++)
        {
            FLASH_FMA_R = FLASH_BASE_ADDR + ( i * 4 );
            FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_ERASE ;
            while (FLASH_FMC_R & FLASH_FMC_ERASE) ;
            //erase
        }

        for(i=0;i<size;i++)
        {
            FLASH_FMD_R = b[i];
            FLASH_FMA_R = FLASH_BASE_ADDR+i;
            FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE ;
            while (FLASH_FMC_R & FLASH_FMC_WRITE) ;
            //write
        }

        for(i=0;i<size;i++)
        {
            a[i] = *(FLASH_BASE_ADDR + i);
            //read
        }

        delay_ms(500);
        uart_7_tx(a[0]);
        uart_7_tx(a[1]);
        //uart_7_tx_str((char*)a[0]);

    }

    asm_rst();

    while(1){}
}



