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




int main()
{
    Sysctl_Clock();
    spi_init_conf();
    uart_init();
    can_init();

    uart_tx_str("Hello can start!!\r\n");
    uart_tx_str("first\r\n");

    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();




    char *b;
    while(1)
    {
        can_rx_conf(0x6b2);
        CanToSpi(&b);
        uart_tx_str(b);
        uart_tx('\n');

        make_character(1,b);
    }



    while(1){}
}


