#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

void uart_init(void)
{

    SYSCTL_RCGCUART_R |=0x00000002;     //UART1 ON(1<<1)
    SYSCTL_RCGCGPIO_R |=0x00000002;     //GPIO PORT B CLOCK ON

    //GPIO_PORTB_LOCK_R =0x4c4f434b;      //GPIO LOCK (DEFAULT)
    //GPIO_PORTB_CR_R |=0x000000FF;       //GPIO COMMIT (DEFAULT)
    GPIO_PORTB_DEN_R |=0x000000FF;      //UART PORT B DIGITAL ENABLE
    GPIO_PORTB_AFSEL_R |=0x00000003;      //PORT B USE ALT FUNC UART
    //GPIO_PORTB_DR2R_R |=IGNORE?;      //LOW I
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&(~(unsigned long)0xFF)) | 0x11;
    //GPIO_PORTB_PCTL_R =  0x******00 | 0x11-> 0x******11;
    //PORT B CONTROL UART FUNC


    UART1_CTL_R =0x00000000;           //UART DISABLE
    UART1_IBRD_R =10;                   //INT PART OF BRD
    UART1_FBRD_R =54;                   //FLOAT PART OF BRD

    UART1_LCRH_R = 0x3 << 5;          //UART CONFIG: 8Bit, No FIFO,No Parity, 1 Stop bit, Etc.
    UART1_CC_R = 0x00000000;           //SYSTEM CLOCK AS UART CLOCK
    UART1_CTL_R = 0x00000301;           //UART ENABLE

}


void uart_tx_hex(int number)
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
    uart_tx_str(&tmpbuffer[index]);
    return;
}

void uart_tx(char data)
{
    while((UART1_FR_R & 0x00000020)!=0);

    UART1_DR_R =data;
}

int uart_tx_str(char *str)
{
    while (*str)
    {
        uart_tx(*str);
        str++;
    }
}



unsigned char uart_rx(void)
{
    while ((UART1_FR_R & 0x00000010)!=0);
    return ((unsigned char)( UART1_DR_R & 0xFF ));
}

void uart_rx_str(char *str,unsigned short max)
{
    int l=0;
    char cha;

    cha=uart_rx();
    if(l<max)
    {
        *str = cha;
        str++;
        l++;
        uart_tx(cha);
    }

    *str =0;

}
