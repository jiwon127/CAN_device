#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Delay.h>
#include <header/testtimer.h>

//

#include <header/asm.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

int timercheck_count;

//yy

uint32_t i=0;


int main()
{
    Sysctl_Clock();

    volatile long int i=0;
    for (i=0;i<10000;i++)
    {   //Delay
        ;
    }
    LCD_ClockInit(); //

    for (i=0;i<35000;i++)
    {   //Delay
        ;
    }

    Timer0_init(0x00030D40);    //timer0 is for distance
                                //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq
                                //20MHz -> 2000000로 해야 0.1s. => (jiwon) = 0x001E8480
                                // 20MHz -> 200000로 하면 0.01s => ecu에서 보내는 주기가 100Hz니까.
                                //(jiwon) = 0x0003\0D40
                                // 130000으로 해야 0.01초가 된다. 0x0001FBD0

    while(1)
    {
        if((timercheck_count / 100) == 1)
        {
            timercheck_count = 0;
            Led(0x02);
            delay_ms(100);
            Led(0x00);
            delay_ms(100);
        }

    }

}
