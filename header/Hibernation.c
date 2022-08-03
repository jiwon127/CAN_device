/*
 * Hibernation.c
 *
 *  Created on: 2021. 4. 27.
 *      Author: kjm-hsh
 */
#include <header/Flash_memory.h>
#include <header/asm.h>
#include <header/Can_init.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

extern uint32_t distance_data[8];
extern uint32_t Engine_distance_data[8];
extern uint32_t Mission_distance_data[8];
extern uint32_t Break_distance_data[8];
extern int hib_count;
extern volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
// yy
extern volatile double Engine_distance2, Mission_distance2, Break_distance2;  //  km
// yy



void hib_wrc()
{
    while(!(HIB_CTL_R & HIB_CTL_WRC)){};
}

void hib_init()
{
    SYSCTL_RCGCHIB_R |= 0x01;
    while((SYSCTL_PRHIB_R&0x01)==0){}; // wait for clock
    hib_wrc();
    HIB_CTL_R = 0x40;  // CLK32EN Hibernation module clock source is enabled
    hib_wrc();
    HIB_CTL_R = 0x140; // VDD3ON internal switches control the power to the on-chip modules

}
void hibernate()
{
    hib_wrc();
    HIB_RTCM0_R = 0x10; // time to sleep

    hib_wrc();
    HIB_RTCSS_R = 0;       // no subseconds time

    hib_wrc();
    HIB_RTCLD_R = 0;       // clear RTC counter

    hib_wrc();
    HIB_CTL_R = 0x0152;    // go to sleep (no rtc wake up)
}



void into_hibernate(int flag)
{
    if (flag == 1)
    {
        LCD_OnOff(0);

        //uart clk
        SYSCTL_RCGCUART_R  &= ~SYSCTL_RCGCUART_R7;     //UART7 clk Off(1<<7)
        SYSCTL_RCGCGPIO_R  &= ~SYSCTL_RCGCGPIO_R4;     //GPIO PORT E CLOCK Off


        //spi clk
        ClearScreen();
        SYSCTL_RCGCSSI_R   &= ~SYSCTL_RCGCSSI_R0;      //selecting ssi1
        SYSCTL_RCGCGPIO_R  &= ~SYSCTL_RCGCSSI_R0;     //1i


        //can clk
        SYSCTL_RCGCCAN_R   &= ~(SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1);   //CAN0, 1 CLOCK Off(1,0 <<1) == Enable CAN0, CAN1
        SYSCTL_RCGCGPIO_R  &= ~SYSCTL_RCGCGPIO_R0;    //GPIO PORT A CLOCK Off

        //timer clk
        SYSCTL_RCGCTIMER_R &= ~SYSCTL_RCGCTIMER_R0 ; //disable timer0 clock gating

        //led clk
        //Led(0);

        SYSCTL_RCGCGPIO_R  &= ~SYSCTL_RCGC2_GPIOD;

        Flash_erase(0,1);
        Flash_write(Engine_distance_data,4,0);
        Flash_erase(64,65);
        Flash_write(Mission_distance_data,4,256);
        Flash_erase(128,129);
        Flash_write(Break_distance_data,4,512);

        hibernate();

        asm_rst();
    }
}



