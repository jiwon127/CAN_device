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
    //uart_init();
    //can_init();

    SYSCTL_RCGCSSI_R = (SYSCTL_RCGCSSI_R&(~(unsigned long)SYSCTL_RCGCSSI_R0))
                        | SYSCTL_RCGCSSI_R0;//1
    SYSCTL_RCGCGPIO_R  = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGCGPIO_R0))
                        | SYSCTL_RCGCSSI_R0;//1

    GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0xff)) | 0x34;
    GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)0xffffffff)) | 0x00220200;

    GPIO_PORTA_DATA_R =(GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) |0x08;

    GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x34)) | 0x34;




    SSI0_CR1_R = 0x0;
    SSI0_CC_R = 0x0;
    SSI0_CPSR_R = 0x2;
    SSI0_CR0_R = 0x907;
    SSI0_CR1_R = 0x2;

    char spi_put_data_byte[5]={0xaa,0x55,0xaa,0x55,0xaa};

    char spi_get_data_byte[5]={10,11,12,13,14,15};

    int i;

    for(i=0;;i++)
    {
        GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x08)) ;
        spi_data_put(spi_put_data_byte[i]);

        while(SSI0_SR_R&SSI_SR_BSY){}
        //spi_data_get(&spi_get_data_byte[i]);
        GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x08)) | 0x08;

        if (i>5) i=0;

    }

    while(1){}

    return 0;
}
