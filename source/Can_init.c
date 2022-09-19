/*
 * Can_init.c
 *
 *  Created on: 2021. 2. 3.
 *      Author: kjm-hsh
 */

#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Delay.h>
#include <header/my_eve2_lib.h>
#include <header/Can_init.h>

#include <header/debugging_h.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>
#include <time.h>

#include <header/timer.h>

extern volatile unsigned int IRQ_A1,IRQ_A2, IRQ_B1, IRQ_B2;
extern volatile uint32_t irq_status,can_flag;
volatile uint32_t sts_test;
extern volatile uint32_t i;
extern volatile uint32_t isr_flag ;
extern int can_check;

uint32_t GetTickCheck;
uint32_t StartTick, Tickcnt;
extern uint32_t EndTick;


void can_init(int bps, int irq)
{
    SYSCTL_RCGCCAN_R   = (SYSCTL_RCGCCAN_R&(~(unsigned long)(SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1)))
                                                                                                    | (SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1);
    //CAN0, 1 CLOCK ON(1,0 <<1) == Enable CAN0, CAN1

    SYSCTL_RCGCGPIO_R  = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGCGPIO_R0))
                                                                                                    | SYSCTL_RCGCGPIO_R0;
    //GPIO PORT A CLOCK ON

    GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x03)) | 0x01|0x02;
    //GPIO_PORTA_DEN_R = 0x000000FF;  //¾ê ¶§¹®¿¡....
    //use the pin as a digital input or output (either GPIO or alternate function), the corresponding GPIODEN bit must be set.

    GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0x03)) | 0x01|0x02;   //Pin0, Pin1
    //PORT A USE ALT FUNC CAN (PA0->1, PA1->1);


    GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)0xFF)) | 0x88;
    //GPIO_PORTA_PCTL_R =  0x******00 | 0x88-> 0x******88;
    //PA0->CANRx, PA1->CANTx

    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_INIT)) | CAN_CTL_INIT;
    //CAN INITIALIZE (put into initialize mode)
    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_CCE)) | CAN_CTL_CCE;    //CCE BIT.
    //CAN Register change enabled.

    if(irq == 1)
    {
        CAN1_CTL_R |= (CAN1_CTL_R&(~(unsigned long)( CAN_CTL_SIE| CAN_CTL_IE))) |
                ( CAN_CTL_SIE| CAN_CTL_IE );

        NVIC_PRI10_R |= 0x00000080; // pri 10
        NVIC_EN1_R = 1<<8; // IRQ
    }
    //CAN Interrupt enable


    //CAN CLOCK: 20Mhz.
    //NOW systemClock = 20Mhz!->We use CAN Clock as 20Mhz, Let us calculate.
    //Note that "The fsys input clock is the system clock frequency as configured by the RCC or RCC2 registers"


    //We set 1Mhz, bitTime=1us, n=4 therefore, tq= 1us/4 = 250ns
    //tq = 250ns
    //tq = 500ns when 500Kbps, 2us, n=4, tq=2/4=500ns

    //BRP = 250ns * 20Mhz = 5
    //500kbps BRP = 5*2;

    //Below are relative value of tQ: no need to change (250->500)
    //tSync = 1*tq = 250ns(->500 when 500kbps)
    //tProp = 1*tq = 250ns(->same as above)

    //bittime = 1us = tSync+Tseg1 +Tseg2 = 4tq
    //tProp+tPhase1+tPhase2 = 3tq.
    //tPhase1= 1tQ.
    //tPahse2 = 1tq.

    //tSeg1 = 2tq,
    //tSe2 = 1tq.

    //TSEG2 = 1-1, TESG1 = 2-1, SJW = 1-1, BRP = 5-1 (10-1 when 500kbps)
    //0<<12, 1<<8,
    if(bps==1000)   CAN1_BIT_R = (CAN1_BIT_R&(~(unsigned long)0xFFFF)) | ( (0<<12) | (1<<8) | (0<<6) | ((5-1)<<0) );
    else if(bps==500) CAN1_BIT_R = (CAN1_BIT_R&(~(unsigned long)0xFFFF)) | ( (0<<12) | (1<<8) | (0<<6) | ((10-1)<<0) );

    CAN1_BRPE_R = 0;    //BPE is enough.
    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_CCE));    //Now no body can change bit rate..
    //Now we have clean CAN clock from here.

    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_INIT));   //Put into Running Mode.
}

void can1_handler(void)
{
    int i=0;
    int bitCnt = 1;
    //        if(Tickcnt == 0)
    //        {
    //            StartTick = GetTickCheck;
    //            Tickcnt++;
    //        }
    //        else
    //        {
    //            Tickcnt = 0;
    //            EndTick = GetTickCheck - StartTick;
    //        }
    for (i=0; i<MAX_ID_NUM; i++)
    {
        if (CAN1_MSG1INT_R & bitCnt)
        {
            if(i == 2)
            {
                if(Tickcnt == 0)
                {
                    StartTick = GetTickCheck;
                    Tickcnt++;
                }
                else
                {
                    Tickcnt = 0;
                    EndTick = GetTickCheck - StartTick;
                }
            }
            CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)CAN_IF1MCTL_INTPND));
            CAN1_IF1CMSK_R =  CAN_IF1CMSK_CLRINTPND | CAN_IF1CMSK_NEWDAT | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB ;
            CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | (i+1);

            while (1)
            {
                if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
                        break;
            }
            (*(short *)(&(globalCanData[i].CANDATA[0])) ) = CAN1_IF1DA1_R;
            (*(short *)(&(globalCanData[i].CANDATA[2])) ) = CAN1_IF1DA2_R;
            (*(short *)(&(globalCanData[i].CANDATA[4])) ) = CAN1_IF1DB1_R;
            (*(short *)(&(globalCanData[i].CANDATA[6])) ) = CAN1_IF1DB2_R;
            globalCanData[i].ifValid = 1;
        }
        bitCnt = bitCnt << 1;
    }
    irq_status = CAN1_STS_R;
}


void can_rx2(int id, char msgBoxNum)    //Purely setup Message Box.
{
    CAN1_IF1CMSK_R = (CAN1_IF1CMSK_R&(~(unsigned long)0xFF)) |
            ( CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_MASK | CAN_IF1CMSK_ARB
                    | CAN_IF1CMSK_CONTROL  | CAN_IF1CMSK_CLRINTPND| CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB);
    CAN1_IF1MSK1_R = 0x0;
    CAN1_IF1MSK2_R = 0x1FFF;
    CAN1_IF1ARB2_R = (CAN1_IF1ARB2_R&(~(unsigned long)0xFFFF)) |
            ( CAN_IF1ARB2_MSGVAL | id<< 2 );

    CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)0xFFFF)) |
            ( //CAN_IF1MCTL_INTPND|
                    CAN_IF1MCTL_RXIE | CAN_IF1MCTL_UMASK
                    | CAN_IF1MCTL_EOB | 8 );
    CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | msgBoxNum;

    while (1)
    {
        if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
            break;
    }

}
