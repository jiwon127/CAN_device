#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/my_eve2_lib.h>
#include <header/Flash_memory.h>

#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


int main()
{
    Sysctl_Clock();

    int Flash_Write_Data[4]={0x12,0x34,0xab,0xcd};
    int *Flash_Read_Data;

    Flash_write(Flash_Write_Data,4);

    int i;

    for( i=0;i<4;i++)
    {
        Flash_Read_Data=Flash_read[i];
    }

    while(1){}
}
