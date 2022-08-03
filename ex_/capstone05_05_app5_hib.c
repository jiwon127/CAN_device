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



int hib_flag, hib_count, timer2_flag;
volatile int speed, int_distance; // km/h
volatile double interval, distance;  //  km
char distance_array[6];

uint32_t distance_data[2];


int main()
{
    Sysctl_Clock();

    can_init(1000);
    //can_init(500);

    while(1)
    {
        can_rx_conf(0x6b2); //transmission

        uint32_t a1_1=CAN1_IF1DA2_R;   //0x97
        uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

        static char DC1[2]={0,};

        DC1[0]=((char)a1_2>>4);
        DC1[1]=((char)a1_2 & 0x0f);

        //uart_7_tx(uart_7_rx());
        if(DC1[1] == 0) continue;
        else  break;
    }
    //waiting for break 'P'

    uart_7_init();
    spi_init_conf();
    hib_init();
    can_init(1000);
    //can_init(500);

    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();

    uart_7_tx_str("start!!\r\n");
    //uart_7_tx(uart_7_rx()); //MUST ADD THIS LINE FOR HIB
    Timer0_init(0x007a1200);    //timer0 is for distance
                                //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq

    Timer2_init(0x04c4b400);    //timer2 is for hib condition check
                                //Interrupt Request every 1s
                                // 0x04c4b400 == 160000000 == 1s irq

    Led(0);


    //Flash_read(distance_data, 1);


    while(1)
    {

        uart_7_tx_str(distance_array);
        uart_7_tx('\n');
        make_character(8, distance_array);
        delay_ms(200);


        if(hib_count >= 5)    //
        {
            into_hibernate(1);
        }

    }


    //asm_rst();
}
