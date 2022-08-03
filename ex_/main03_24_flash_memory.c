#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/my_eve2_lib.h>

#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



#define FLASH_BASE_ADDR         ((volatile uint32_t*)0x00020000)

int main()
{
    Sysctl_Clock();

    uint32_t a[2],b[2];

    while(1)
    {
        FLASH_FMA_R = FLASH_BASE_ADDR ;
        FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_ERASE ;
        while (FLASH_FMC_R & FLASH_FMC_ERASE) {}
        a[0 ] = FLASH_BASE_ADDR[0];

        FLASH_FMD_R = 0x00000004;
        FLASH_FMA_R =  FLASH_BASE_ADDR;
        FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE ;
        while (FLASH_FMC_R & FLASH_FMC_WRITE) ;
        a[1] = FLASH_BASE_ADDR[0];


    }

    while(1){}
}



