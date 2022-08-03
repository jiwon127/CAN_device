/*
 * testtimer.c
 *
 *  Created on: 2022. 5. 16.
 *      Author: esllab
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

extern int timercheck_count;

void Timer0_init(uint32_t period)
{
    //TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0 ;
    //Enable timer0 clock gating
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //Disable before making change
    TIMER0_CFG_R =0 ;
    //For a 16/32-bit timer, this value selects the 32-bit timer configuration
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;
    //Periodic Timer mode
    TIMER0_TAILR_R = period-1;
    //Load the start value
    TIMER0_TAPR_R = 0;
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
    //Writing a 1 to this bit clears the TATORIS bit in the GPTMRIS register
    //and the TATOMIS bit in the GPTMMIS register.
    TIMER0_IMR_R |= TIMER_IMR_TATOIM;
    //Interrupt mask
    NVIC_PRI4_R |= 0x10000000;
    //irq
    NVIC_EN0_R =  1 << 19;
    //irq
    TIMER0_CTL_R = TIMER_CTL_TAEN;
    //Start counting
}



void Timer0_disable()
{
    TIMER0_CTL_R &= ~0x01;  // disable Timer2A during setup
}

void Timer0_clear_irq(void)
{
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;
}

void timer0_handler(void)   //convert receive can data to char ptr for display ISR
{
    timercheck_count++;
}

