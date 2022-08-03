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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


int main()
{
    Sysctl_Clock();
    spi_init_conf();
    uart_init();
    uart_7_init();

    uart_7_tx_str("Hello can start!!\r\n");
    uart_7_tx_str("first\r\n");

    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();

    int i=0;
    int length=32;
    char a[32]={0,};
    char *b = a;

    while(1)
    {
        a[0] = uart_7_rx();
        if (a[0] =='\n') continue;
        else if (a[0]=='\r') continue;

        for(i=1;i<length;i++)
        {

            a[i] = uart_7_rx();

            if((a[i] == '\r')||(a[i] == '\n'))
            {
                a[i]=0;
                break;
            }

        }

        uart_7_tx_str(a);

        if (a[0] == 'z')
        {
            ClearScreen();
            asm_rst();
        }

        make_character(1,b);
        if( *b == 'p') break;


    }

    make_character(0,"communication finish"); //20 character

    while(1){}
}


