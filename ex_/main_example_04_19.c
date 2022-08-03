/*
 * main.c
 *
 *  Created on: 2021. 2. 1.
 *      Author: kjm-hsh
 */

#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Gpio_led.h>
#include <header/Delay.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



int main(void)
{

    Sysctl_Clock();

    while (1)
    {

        Led(0xaa);
        delay_ms(500);
        Led(0x55);
        delay_ms(500);
        Led(0x00);
        delay_ms(500);
        Led(0xff);
        delay_ms(500);

    }


    return 0;
}
