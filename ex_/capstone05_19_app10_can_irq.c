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

#define DEBUG_TRANSMISSION_CAN_ID 0x6b2
#define DEBUG_RPM_CAN_ID 0x6b2
#define DEBUG_ACCELERATION_CAN_ID 0x6b2

#define TRANSMISSION_CAN_ID 0x43f
#define RPM_CAN_ID 0x316
#define ACCELERATION_CAN_ID 0x440

int hib_flag, hib_count, timer2_flag;
volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
volatile uint32_t irq_status,can_flag;

char distance_array[16]={0,};
char Engine_distance_array[16]={0,};
char Mission_distance_array[16]={0,};
char Break_distance_array[16]={0,};


int *speed_ptr;

uint32_t distance_data[sizeof(distance)]={0,};
uint32_t distance_data_clear[sizeof(distance)]={0,};

uint32_t Engine_distance_data[sizeof(Engine_distance)]={0,};
uint32_t Mission_distance_data[sizeof(Mission_distance)]={0,};
uint32_t Break_distance_data[sizeof(Break_distance)]={0,};

uint32_t i=0;
hib_count = 0;

CANDATA_T globalCanData[MAX_ID_NUM];

int main()
{
    Sysctl_Clock();

    can_init(1000,1);     //Debugging
    //when go to car must check can_to_spi_transmission, etc
    //can_init(500);

    //check_wake_state(DEBUG_TRANSMISSION_CAN_ID);     //Debugging
    //waiting for break 'P'

    uart_7_init();
    spi_init_conf();
    hib_init();
    //can_init(1000,0);     //Debugging
    //can_init(500);

    SPI_LCD_ACTIVE();
    FT81x_Init();
    ClearScreen();
    hongCalibrate();

/*
    Flash_erase(0,63);
    Flash_write(distance_data_clear,4,0);
    Flash_erase(64,127);
    Flash_write(distance_data_clear,4,256);
    Flash_erase(128,255);
    Flash_write(distance_data_clear,4,512);
*/
    //the above lines reset flash memory

    Flash_read( Engine_distance_data,  4, 0);
    Flash_read( Mission_distance_data,  4, 256);
    Flash_read( Break_distance_data,  4, 512);

    memcpy(&Engine_distance, Engine_distance_data, sizeof(Engine_distance));
    memcpy(&Mission_distance, Mission_distance_data, sizeof(Mission_distance));
    memcpy(&Break_distance, Break_distance_data, sizeof(Break_distance));
    //copy distance to distance_data

    Timer0_init(0x007a1200);    //timer0 is for distance
                                //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq

    Timer2_init(0x04c4b400);    //timer2 is for hib condition check
                                //Interrupt Request every 1s
                                // 0x04c4b400 == 160000000 == 1s irq
    Led(0);
    uart_7_tx_str("start\r\n");

    char *Gear,*Rpm,*Speed;
    uint8_t pressed = 0;
    uint8_t point_display = 1;
    uint8_t rst_flag = 0;
    uint8_t yes_flag = 0;

    while(1)
    {

        uint8_t Tag = spi_read_8(REG_TOUCH_TAG + RAM_REG);  //Check for touches
        uart_7_tx(Tag+'0');

        ascii_Engine_distance_array();
        ascii_Mission_distance_array();
        ascii_Break_distance_array();

        //can_to_spi_rpm(&Rpm, *(short *)(&(globalCanData[0].CANDATA[0])));
        //can_to_spi_transmission(&Gear,*(short *)(&(globalCanData[1].CANDATA[2])));
        //can_to_spi_acceleration(&Speed,*(short *)(&(globalCanData[2].CANDATA[2])));

        can_to_spi_rpm2(DEBUG_RPM_CAN_ID,&Rpm);
        can_to_spi_transmission2(DEBUG_TRANSMISSION_CAN_ID,&Gear);
        can_to_spi_acceleration2(DEBUG_ACCELERATION_CAN_ID,&Speed);

        while(!(speed >= 10))
        {
            display_fuel(20, Engine_distance_array, Mission_distance_array, Break_distance_array);
            delay_ms(50);
        }


        if(point_display == 0)
            display_accel_trans_rpm(20,Gear,Rpm,Speed);
        else if(point_display == 1)
            display_fuel(20, Engine_distance_array, Mission_distance_array, Break_distance_array);
        else if(point_display == 2 )
            display_fuel_reset(20 ,Engine_distance_array, Mission_distance_array, Break_distance_array);
        else if(point_display == 8 )
            display_reset_choice();

        if(yes_flag == 1 )
        {

            if(rst_flag == 1 )
            {
                make_character(0,"Engine oil reset ok!");
                Flash_erase(0,1);
                Flash_write(distance_data_clear,4,0);
                Flash_read( Engine_distance_data,  4, 0);
                memcpy(&Engine_distance, Engine_distance_data, sizeof(Engine_distance));
            }
            else if(rst_flag == 2)
            {
                make_character(0,"Mission oil reset ok!");
                Flash_erase(64,65);
                Flash_write(distance_data_clear,4, 256);
                Flash_read( Mission_distance_data,  4, 256);
                memcpy(&Mission_distance, Mission_distance_data, sizeof(Mission_distance));
            }
            else if(rst_flag == 3)
            {
                make_character(0,"Break oil reset ok!");
                Flash_erase(128,129);
                Flash_write(distance_data_clear,4, 512);
                Flash_read( Break_distance_data,  4, 512);
                memcpy(&Break_distance, Break_distance_data, sizeof(Break_distance));
            }

            delay_ms(1000);
            yes_flag =0;
            rst_flag =0;
            point_display = 1;

            uart_7_tx('r');

        }

        if(hib_count  == 30)
        {
            uart_7_tx_str("Hibernate\r\n");
            into_hibernate(1);
        }

        if((hib_count % 10) == 0)
        {
            uart_7_tx_str("Data save\r\n");
            Flash_save();
        }


        switch (Tag)
        {
            case 1:         //display distance & oil screen
                if (!pressed)   point_display = 1;  //display_fuel
                break;
            case 2:         //display accel, gear, rpm screen
                if (!pressed)   point_display = 0;   //display_accel_trans_rpm
                break;
            case 3:         //display reset screen
                if (!pressed)   point_display = 2;   //display_accel_trans_rpm
                break;
            case 4:         //engine oil reset button
                if (!pressed)
                {
                    rst_flag = 1;
                    point_display = 8;
                }
                break;
            case 5:         //break oil reset button
                if (!pressed)
                {
                    rst_flag = 2;
                    point_display = 8;
                }
                break;
            case 6:         //mission oil reset button
                if (!pressed)
                {
                    rst_flag = 3;
                    point_display = 8;
                }
                break;
            case 7:         //Back button
                if (!pressed)   point_display = 1;  //display_fuel_back
                break;
            case 8:         // Yes button
                if (!pressed)   yes_flag = 1;
                break;
            case 9:         // No button
                if (!pressed)   point_display = 2;  //display_fuel
                break;

            default:
                if (pressed)
                {
                    make_character(20,"Error!!");
                    delay_ms(1000);
                }
                break;
        }
    }
}


