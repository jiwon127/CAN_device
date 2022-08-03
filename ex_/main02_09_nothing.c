#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

int main(void)
{
    Sysctl_Clock();
    uart_init();
    can_init();

    uart_tx_str("Hello can start!!\r\n");
    Led(0xff);



    while(1)
    {

    }

    return 0;
}

