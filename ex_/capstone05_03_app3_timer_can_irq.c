#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/timer.h>

#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/my_eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


volatile int speed; // km/h
volatile double interval, distance;  //  km

void timer0_handler(void)   //ISR
{
    //uart_7_tx(uart_7_rx());
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; //clear the interrupt request
    GPIO_PORTD_DEN_R ^= 0xaa;


    can_rx_conf(0x6b2); //Speed

    //uint32_t a2_1=CAN1_IF1DA2_R; //a2_1 is Third byte
    //Byte 0 is the first byte to be transmitted or received and data byte 7 is the last byte

    uint32_t a2_1=CAN1_IF1DA1_R;   //0x00
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B

    static char DD1[2]={0,};

    DD1[0]=((char)a2_1>>4);     //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> >>4 == 0x0012
    DD1[1]=((char)a2_1 & 0x0f); //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> &0x0f == 0x0034


    speed=DD1[0]*16+DD1[1]; //hex to decimal
    interval = speed * 2.7778e-5;// ∞ÌπŒ¡ﬂ
    distance += interval;

    uart_7_tx(distance+'0');
}

void timer2_handler(void)   //ISR
{}

int main()
{
    Sysctl_Clock();
    uart_7_init();
    spi_init_conf();
    can_init();

    uart_7_tx_str("start!!\r\n");

    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();

    char a=0;
    char *b=(char)distance;

    Led(0xaa);
    Timer0_init(0x007a1200);    //Interrupt Request every 0.1s
    Timer2_init(0x007a1200);    //Interrupt Request every 0.1s

    Led(0);



    while(1)
    {
        //printReceivedCanData(0x6b2);
        //CanToSpiSpeed(&b);
        //uart_7_tx_str(b);
        //uart_7_tx('\n');
        //make_character(1,b);
        //delay_ms(500);
    }


    asm_rst();
}

