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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>




int main()
{
    Sysctl_Clock();
    uart_init();
    uart_7_init();

    uart_7_tx_str("Hello can start!!\r\n");
    uart_7_tx_str("first\r\n");

    uart_7_tx_str("press any key :");
    uart_7_tx(uart_7_rx()); //MUST ADD THIS LINE FOR HIB
    uart_7_tx_str("\r\n");

    hib_init();
    hibernate();


    delay_ms(4000);

    asm_rst();

    while(1){}
}





