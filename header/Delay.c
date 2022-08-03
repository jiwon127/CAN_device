#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


void delay_us(unsigned int us)
{
    while (us)
    {
        __delay_cycles(20); // 1 for 1 Mhz set 16 for 16 MHz
        us--;
    }
}

void delay_ms(unsigned int ms)
{
    while (ms)
    {
        delay_us(1000); //1000 for 1MHz and 16000 for 16MHz
        ms--;
    }
}

