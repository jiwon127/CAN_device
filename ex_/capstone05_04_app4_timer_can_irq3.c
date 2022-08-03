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



#define hundread_thousand 100000
#define ten_thousand 10000
#define thousand 1000
#define hundread 100
#define ten 10
#define one 1




volatile int speed, int_distance; // km/h
volatile double interval, distance;  //  km
char distance_array[6];

uint32_t distance_data[8];

void timer0_handler(void)   //convert receive can data to char ptr for display ISR
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
    interval = speed * 2.7778e-3;// real value = e-5 but it's too late to test so i choose e-2
    distance += interval;


    int_distance = (int)distance;
    if ((int_distance / hundread_thousand)==0)  distance_array[0] = '0';
    else distance_array[0] = int_distance / hundread_thousand +'0';

    if ((int_distance % hundread_thousand) / ten_thousand ==0)  distance_array[1] = '0';
    else distance_array[1] = (int_distance % hundread_thousand) / ten_thousand +'0';

    if (((int_distance % hundread_thousand) % ten_thousand) / thousand ==0)  distance_array[2] = '0';
    else distance_array[2] = ((int_distance % hundread_thousand) % ten_thousand ) / thousand+'0';

    if ((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread==0)  distance_array[3] = '0';
    else distance_array[3] = (((int_distance % hundread_thousand) % ten_thousand ) % thousand) /hundread +'0';

    if (((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)  distance_array[4] = '0';
    else distance_array[4] = ((((int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) /ten+'0';

    if ((((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one==0)  distance_array[5] = '0';
    else distance_array[5] = (((((int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) %ten) /one+'0';


    //char distance_array[sizeof(distance)];
    //memcpy(distance_array,&distance,sizeof(distance));

    //uart_7_tx_str("z\r\n");


}

void timer2_handler(void)   //check gear, speed, rpm and enter hibernate ISR
{
/*
     //uart clk
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R7;     //UART7 ON(1<<7)
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;     //GPIO PORT E CLOCK ON


    //can clk
    SYSCTL_RCGCCAN_R  |= (SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1);   //CAN0, 1 CLOCK ON(1,0 <<1) == Enable CAN0, CAN1
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;    //GPIO PORT A CLOCK ON

    SYSCTL_RCGCSSI_R   = SYSCTL_RCGCSSI_R0;
       //selecting ssi1
    SYSCTL_RCGCGPIO_R  = SYSCTL_RCGCSSI_R0;//1i

    Flash_write(distance_data,2);

    hibernate();

    asm_rst();
*/
}






int main()
{
    Sysctl_Clock();
    uart_7_init();
    spi_init_conf();
    hib_init();
    can_init(1000);
    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();
    Timer0_init(0x007a1200);    //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq
                                //Timer2_init(0x007a1200);    //Interrupt Request every 0.1s
    //Flash_read(distance_data, 2);

    uart_7_tx_str("start!!\r\n");



    while(1)
    {
        //printReceivedCanData(0x6b2);
        //CanToSpiSpeed(&b);
        //uart_7_tx_str(b);
        //uart_7_tx('\n');
        //make_character(8, distance_array);
        //delay_ms(500);

        uart_7_tx_str(distance_array);
        uart_7_tx('\n');
        make_character(8, distance_array);
        delay_ms(200);


    }


    asm_rst();
}

