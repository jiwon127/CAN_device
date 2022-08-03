/*
 * main.c
 *
 *  Created on: 2021. 4. 19.
 *      Author: hsh-pjw
 */

#include <header/Sysctl_clk.h>
#include <header/Uart7.h>
#include <header/Gpio_led.h>
#include <header/Delay.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



int main(void)

{

    Sysctl_Clock();
    uart_7_init();


    uart_7_tx_str("Hello World!\r\n");

    while (1)
    {

        Led(0xaa);
        delay_ms(50);
        Led(0x55);
        delay_ms(50);
        Led(0xff);
        delay_ms(50);
        Led(0x00);
        delay_ms(50);


        uart_7_tx(uart_7_rx());

    }


    return 0;
}
