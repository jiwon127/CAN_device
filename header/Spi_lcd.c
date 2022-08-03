/*
 * Spi_lcd.c
 *
 *  Created on: 2021. 2. 15.
 *      Author: kjm-hsh
 */


#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

void spi_init(void)
{
    //init

    SYSCTL_RCGCSSI_R = (SYSCTL_RCGCSSI_R&(~(unsigned long)SYSCTL_RCGCSSI_R0))
                            | SYSCTL_RCGCSSI_R0;
    //1. Enable and provide a clock to SSI module 0 in Run mode.
    SYSCTL_RCGCGPIO_R  = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGCGPIO_R0))
                            | SYSCTL_RCGCGPIO_R0;
    //2. Enable and provide a clock to GPIO Port A in Run mode
    //CAN������ A PORT CLOCK ���ִµ� ����������?

    GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0x34)) | 0x34;
    //3. PORT A USE ALT FUNC CAN (PA2~5->1)
    //BUT THIS IS DEFAULT STATE
    //But But... PA5 must be gpio

    //GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)0x00FFFF00)) | 0x00222200;
    //4. PORT A PMC IS SSI
    //PA2->SSI0Clk, PA3->SSI0Fss, PA4->SSI0Rx, PA5->SSI0Tx
    //BUT THIS IS DEFAULT STATE

    GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x34)) | 0x34;
    //5. The digital functions for the corresponding pin are enabled.
    //0x3C = 0011.1100 PA2~5
}


void spi_conf(void)
{
    //conf

    SSI0_CR1_R = (SSI0_CR1_R&(~(unsigned long)SSI_CR1_SSE));
    //1. SSI operation is disabled to make change.
    SSI0_CR1_R = 0x0;
    //2. master operation
    SSI0_CC_R = 0x0;
    //3. Use system clock (based on clock source and divisor factor)
    SSI0_CPSR_R = (SSI0_CPSR_R&(~(unsigned long)SSI_CPSR_CPSDVSR_M)) | 0x2;
    //4. SSI Clock Prescale Divisor
    //SSInClk = SysClk / (CPSDVSR * (1 + SCR))
    //CPSDVSR=0x2, SCR =0x9
    SSI0_CR0_R = (SSI0_CR0_R&(~(unsigned long)SSI_CR0_SCR_M | SSI_CR0_SPH | SSI_CR0_SPO | SSI_CR0_DSS_M))
                    | 9<<8 | SSI_CR0_SPH | SSI_CR0_SPO | SSI_CR0_DSS_8;
    //5. scr, sph, spo, frf, dss  configure
    //SSI Serial Clock Rate = SCR =0x9
    //SSI Serial Clock Phase , SSI Serial Clock Polarity
    //Freescale SPI mode (SPO=1, SPH=1)
    //1 Mbps bit rate
    //SSI Data Size Select= 8 data bit
    SSI0_CR1_R = (SSI0_CR1_R&(~(unsigned long)SSI_CR1_SSE)) | SSI_CR1_SSE ;
    //5. SSI operation is enabled.

}

void spi_init_conf(void)
{
   SYSCTL_RCGCSSI_R = (SYSCTL_RCGCSSI_R&(~(unsigned long)SYSCTL_RCGCSSI_R0))
                           | SYSCTL_RCGCSSI_R0;
   //selecting ssi1
   SYSCTL_RCGCGPIO_R  = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGCSSI_R0))
                       | SYSCTL_RCGCSSI_R0;//1i


   //providing clk to gpio a
   GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0x34))
                       | 1<<5 | 1<<4 | 1<<2;
   //selecting alt func
   GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)0x00220200))
                       | 0x00220200;
   //selecting ssi as alt func
   GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x34))
                       | 1<<5 | 1<<4 | 1<<2;
   //enable digital mode
   GPIO_PORTA_PUR_R = (GPIO_PORTA_PUR_R&(~(unsigned long)0x34))
                       | 1<<5 | 1<<4 | 1<<2;


   GPIO_PORTA_DIR_R =(GPIO_PORTA_DIR_R&(~(unsigned long)0x08))
                       | 1<<3 ;
   GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0x08))
                       | 0<<3 ;
   GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x08))
                       | 1<<3;
   GPIO_PORTA_PUR_R = (GPIO_PORTA_PUR_R&(~(unsigned long)0x08))
                       | 1<<3;
   GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08))
                       | 1<<3;

   SSI0_CR1_R = 0x0;
   SSI0_CC_R = 0x0;
   SSI0_CPSR_R = 0x2;
   SSI0_CR0_R = 0x907; //0x907
   SSI0_CR1_R = 0x2;
}

void spi_send_byte(char data)
{

    SSI0_DR_R = data;
    while(!( SSI0_SR_R & SSI_SR_TFE)) {}
}

short spi_receive_byte(void)
{
    volatile short data = 0;
    while(!( SSI0_SR_R & SSI_SR_RNE)) {}
     data = SSI0_DR_R ;
     return data;
}

void spi_cs_enable(void)
{
    GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) | 0<<3;  //nCS = 0;
}

void spi_cs_disable(void)
{
    GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) | 1<<3;  //nCS = 1; End of communication;
}


void spi_send_str(char *buffer)
{
    /*
    while(*buffer!=0)
    {
        spi_send_byte(*buffer);
        buffer++;
    }
    */
}

void spi_writebuffer(uint8_t *Buffer, uint32_t Length)
{
    /*
    spi_cs_enable();
    //SSI0_DR_R = data;
    while(!( SSI0_SR_R & SSI_SR_TFE)) {}
    spi_cs_disable();
    */
}
