/*
 * WakePinDATA.c
 *
 *  Created on: 2022. 4. 12.
 *      Author: esllab
 */
#include <tm4c123gh6pm.h>
#include <stdint.h>


void WakePin_Init(void)
{   //INitialize LCD with Power == ON.

    SYSCTL_RCGCGPIO_R    = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGC2_GPIOF)) | SYSCTL_RCGC2_GPIOF;
    //Gpio port F Clock on

    GPIO_PORTF_DIR_R     = (GPIO_PORTD_DIR_R&(~(unsigned long)0xFF)) | 0x00;
    //Gpio port F3 Input mode

    GPIO_PORTF_AFSEL_R   = (GPIO_PORTD_AFSEL_R&(~(unsigned long)0xFF));
    //Don't use alternative function

    //GPIO_PORTF_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0xFF))| 0x08;

    GPIO_PORTF_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x08;
    //Digital function is enabled
}

