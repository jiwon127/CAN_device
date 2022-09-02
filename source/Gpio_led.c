#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



void Led(int pin)
{
    SYSCTL_RCGCGPIO_R    = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGC2_GPIOD)) | SYSCTL_RCGC2_GPIOD;
    //Gpio port D Clock on
    GPIO_PORTD_DIR_R     = (GPIO_PORTD_DIR_R&(~(unsigned long)0xFF)) | 0xFE;
    //Gpio port D 0 ~ 7 output
    GPIO_PORTD_AFSEL_R   = (GPIO_PORTD_AFSEL_R&(~(unsigned long)0xFF));
    //Don't use alternative function

    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x00;

    GPIO_PORTD_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0xFF))| 0xFF;

    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| pin;
    //Digital function is enabled
}

void gpioinit(void)
{
    SYSCTL_RCGCGPIO_R    = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGC2_GPIOD)) | SYSCTL_RCGC2_GPIOD;
    // ������Ϸ��� pin�� port D [0, 1]�̴�. ���� port D�� Ȱ��ȭ �����ش�.

    GPIO_PORTD_DIR_R     = (GPIO_PORTD_DIR_R&(~(unsigned long)0xFF)) | 0xFF;
    // �� �� 1�� SET

    GPIO_PORTD_AFSEL_R   = (GPIO_PORTD_AFSEL_R&(~(unsigned long)0xFF));
    //Don't use alternative function

    GPIO_PORTD_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0xFF))| 0xFE;
    // �� 00 1 0

    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x01;
    // �� ��������� LCD on
}


void LCD_OnOff(int onOff) // pin = 1 => LCD Power On, pin = 0 => LCD Power Off
{
    if (onOff == 1) GPIO_PORTD_DATA_R    = (GPIO_PORTD_DATA_R &(~(unsigned long)0x01));
    else            GPIO_PORTD_DATA_R    = GPIO_PORTD_DATA_R | 0x01;
}

void LED_OnOff(int On1Off2)
{
    if (On1Off2 == 1) GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x03;
    else              GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x01;
}
