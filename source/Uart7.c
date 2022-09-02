#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


void uart_7_init(void)
{

    SYSCTL_RCGCUART_R |=SYSCTL_RCGCUART_R7;     //UART7 ON(1<<7)
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R4;     //GPIO PORT E CLOCK ON

    //GPIO_PORTB_LOCK_R =0x4c4f434b;      //GPIO LOCK (DEFAULT)
    //GPIO_PORTB_CR_R |=0x000000FF;       //GPIO COMMIT (DEFAULT)
    GPIO_PORTE_DEN_R |=0x00000003;      //UART PORT E DIGITAL ENABLE
    GPIO_PORTE_AFSEL_R |=0x00000003;      //PORT E USE ALT FUNC UART
    //GPIO_PORTB_DR2R_R |=IGNORE?;      //LOW I
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&(~(unsigned long)0xFF)) | 0x11;
    //GPIO_PORTE_PCTL_R =  0x******00 | 0x11-> 0x******11;
    //PORT E CONTROL UART FUNC


    UART7_CTL_R =0x00000000;           //UART DISABLE
    UART7_IBRD_R =10;                   //INT PART OF BRD
    UART7_FBRD_R =54;                   //FLOAT PART OF BRD

    UART7_LCRH_R = 0x3 << 5;          //UART CONFIG: 8Bit, No FIFO,No Parity, 1 Stop bit, Etc.
    UART7_CC_R = 0x00000000;           //SYSTEM CLOCK AS UART CLOCK
    UART7_CTL_R = 0x00000301;           //UART ENABLE

}

void uart_7_tx_hex(int number)
{
    char tmpbuffer[21]={0,};
    int index=19;
    for (;index>=0;index--)
    {
        int now = number%0x10;
        if (now>9)
            now=now-10+'A';
        else
            now=now+'0';
        tmpbuffer[index]=now;
        number=number/0x10;

        if (number == 0)
            break;
    }
    index--;
    tmpbuffer[index]='x';
    index--;
    tmpbuffer[index]='0';
    uart_7_tx_str(&tmpbuffer[index]);
    return;
}

void uart_7_tx(char data)
{
    while((UART7_FR_R & 0x00000020)!=0);

    UART7_DR_R =data;
}

int uart_7_tx_str(char *str)
{
    while (*str)
    {
        uart_7_tx(*str);
        str++;
    }
}



unsigned char uart_7_rx(void)
{
    while ((UART7_FR_R & 0x00000010)!=0);
    return ((unsigned char)( UART7_DR_R & 0xFF ));
}

void uart_7_rx_str(char *str,unsigned short max)
{
    int l=0;
    char cha;

    cha=uart_7_rx();
    if(l<max)
    {
        *str = cha;
        str++;
        l++;
        uart_7_tx(cha);
    }

    *str =0;

}
