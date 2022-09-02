/*
 * PMOS_LCD_Control.c
 *
 *  Created on: 2022. 3. 25.
 *      Author: esllab
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

/*
void LCD_ClockInit(void)
{   //INitialize LCD with Power == ON.
    SYSCTL_RCGCGPIO_R    = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGC2_GPIOD)) | SYSCTL_RCGC2_GPIOD;
    //Gpio port D Clock on
    GPIO_PORTD_DIR_R     = (GPIO_PORTD_DIR_R&(~(unsigned long)0xFF)) | 0x1;
    //Gpio port D 0 ~ 7 output
    GPIO_PORTD_AFSEL_R   = (GPIO_PORTD_AFSEL_R&(~(unsigned long)0xFF));
    //Don't use alternative function
    GPIO_PORTD_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0xFF))| 0x00;

    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x1;
    //Digital function is enabled
}

void LCD_OnOff(int onOff) // pin = 1 => LCD Power On, pin = 0 => LCD Power Off
{
    if (onOff == 1) GPIO_PORTD_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0x01));
    else            GPIO_PORTD_DATA_R    = GPIO_PORTD_DATA_R | 0x01;
}
*/
