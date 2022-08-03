#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

int main(void)
{
    Sysctl_Clock();
    uart_init();
    can_init();
    spi_init();
    spi_conf();

    uart_tx_str("Hello can start!!\r\n");
    Led(0xff);



    GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)GPIO_PCTL_PA3_M)) | 0x0000;
    //1. fss -> gpio
    GPIO_PORTA_DIR_R     = (GPIO_PORTA_DIR_R&(~(unsigned long)0x08)) | 0x08;
    //1-1. dir = 1 (output)
    GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x4)) | 0x4;
    //GPIO_PORTA_DATA_R   =(GPIO_PORTA_DATA_R &(~(unsigned long)0x08));
    //2. PA3 = 0 (for activate fss)
    GPIO_PORTA_DATA_R   =0;

    char spi_put_data_byte[5]={1,2,3,4,5};

    char spi_get_data_byte[5]={10,11,12,13,14,15};

    int i;

    for(i=0;i<5;i++)
    {
        spi_data_put(spi_put_data_byte[i]);
    }
    //3. write 8bit 5byte


    for(i=0;i<5;i++)
    {
        spi_data_get(&spi_get_data_byte[i]);
    }
    //4. read 8bit 5byte


    GPIO_PORTA_DATA_R   =(GPIO_PORTA_DATA_R &(~(unsigned long)0x08))| 0x08;
    //5. PA3 = 1 (for inactivate fss)



    while(1) {
        GPIO_PORTA_DATA_R   =(GPIO_PORTA_DATA_R &(~(unsigned long)0x08))| 0x08;
        GPIO_PORTA_DATA_R   =(GPIO_PORTA_DATA_R &(~(unsigned long)0x08));
    }

    return 0;
}

