#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Delay.h>
#include <header/timer.h>



#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

#define TOUCH_TPR 1

#define DEBUG_TRANSMISSION_CAN_ID 0x6b2
#define DEBUG_RPM_CAN_ID 0x6b2
#define DEBUG_ACCELERATION_CAN_ID 0x3cb

#define TRANSMISSION_CAN_ID 0x316
#define RPM_CAN_ID 0x43f
#define ACCELERATION_CAN_ID 0x440


int hib_flag, hib_count, timer2_flag;
volatile int speed, int_distance; // km/h
volatile double interval, distance;  //  km
char distance_array[5]={0,0,0,0,0};

uint32_t distance_data[sizeof(distance)]={0,};


static uint8_t Touch;

uint8_t Display_Touch()
{
    return Touch;
}



int main()
{
    Sysctl_Clock();


    can_init(1000,1);   //Debugging

    check_wake_state(0x43f);
    //waiting for break 'P'

    uart_7_init();
    spi_init_conf();
    hib_init();




    uart_7_tx_str("start!!\r\n");

    uint32_t i;



    memcpy(&distance,distance_data,sizeof(distance_data));
    //copy distance to distance_data

    Timer0_init(0x007a1200);    //timer0 is for distance
                                //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq

    Timer2_init(0x04c4b400);    //timer2 is for hib condition check
                                //Interrupt Request every 1s
                                // 0x04c4b400 == 160000000 == 1s irq

    Led(0);


    char *Gear,*Rpm,*Speed;


    while(1)
    {

        can_to_spi_rpm(DEBUG_TRANSMISSION_CAN_ID,&Gear);
        can_to_spi_transmission(RPM_CAN_ID,&Rpm);
        can_to_spi_acceleration(ACCELERATION_CAN_ID,&Speed);

        display_accel_trans_rpm(1,Gear,Rpm,Speed);
        delay_ms(600);


        uart_7_tx_str(distance_array);   //Debugging
        uart_7_tx('\n');   //Debugging
        //display_fuel(0, distance_array, distance_array, distance_array);
        delay_ms(600);


        if(hib_count >= 8)    //
        {
            into_hibernate(1);
        }

    }


    //asm_rst();
}







