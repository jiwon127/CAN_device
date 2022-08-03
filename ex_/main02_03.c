/*
 * exam.c
 *
 *  Created on: 2021. 2. 2.
 *      Author: kjm-hsh
 */

#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
//#include <header/Can_init.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

int main(void)
{
    Sysctl_Clock();
    uart_init();
    //can_init(500, 0);

    uart_tx_str("Hello can start!!\r\n");
    Led(0xaa);
   // can_rx_conf(0x3cb);
    //printReceivedCanData(1);
    Led(0x55);

    //can_tx_conf();

    while(1)
    {
        Led(0x55);
       // can_rx_conf(0x6b2);
       // printReceivedCanData(1);

      //  can_rx_conf(0x3b);
       // printReceivedCanData(1);

       // can_rx_conf(0x3cb);
       // printReceivedCanData(1);
        Led(0xaa);


        uart_tx_str("abcd\r\n");
    }

    return 0;
}

