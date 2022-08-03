/*
 * main02_04.c
 *
 *  Created on: 2021. 2. 4.
 *      Author: kjm-hsh
 */

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
    Led(0xaa);
    can_rx_conf(0x3cb);
    printReceivedCanData();
    Led(0x55);

   // can_tx_conf(0xb2);

    while(1)
    {
        Led(0x55);
        can_rx_conf(0x6b2);
        printReceivedCanData();

        can_rx_conf(0x3b);
        printReceivedCanData();

        can_rx_conf(0x3cb);
        printReceivedCanData();
        Led(0xaa);


        uart_tx_str("abcd\r\n");
    }

    return 0;
}

