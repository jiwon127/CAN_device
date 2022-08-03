
#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/timer.h>

//
#include <header/PMOS_LCD_Control.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/eve2_lib.h>
#include <header/my_eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <header/WakePinData.h>

#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


//#pragma src



#define DEBUG_TRANSMISSION_CAN_ID 0x3b
#define DEBUG_RPM_CAN_ID 0x3cb
#define DEBUG_ACCELERATION_CAN_ID 0x6B2
// 3b , 3cb, 6b2
#define TRANSMISSION_CAN_ID 0x43f
#define RPM_CAN_ID 0x316
#define ACCELERATION_CAN_ID 0x440

int hib_flag, hib_count, flash_count, timer2_flag, timercheck_count,timercheck_count2;
volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
volatile unsigned int IRQ_A1,IRQ_A2, IRQ_B1, IRQ_B2;
volatile uint32_t irq_status,can_flag, isr_flag = 0;

char distance_array[16]={0,};
char Engine_distance_array[16]={0,};
char Mission_distance_array[16]={0,};
char Break_distance_array[16]={0,};

int *speed_ptr;

uint32_t distance_data[sizeof(distance)]={0,};
uint32_t distance_data_clear[sizeof(distance)]={0,};

/*
uint32_t Engine_distance_data[sizeof(Engine_distance)]={0,};
uint32_t Mission_distance_data[sizeof(Mission_distance)]={0,};
uint32_t Break_distance_data[sizeof(Break_distance)]={0,};
*/
//yy
volatile double Engine_distance2, Mission_distance2, Break_distance2;  //  km
volatile double Engine_interval, Mission_interval, Break_interval;

uint32_t Engine_distance_data[sizeof(Engine_distance2)]={0,};
uint32_t Mission_distance_data[sizeof(Mission_distance2)]={0,};
uint32_t Break_distance_data[sizeof(Break_distance2)]={0,};
//yy

volatile uint32_t i=0;
hib_count = 0;
flash_count = 0;
timercheck_count = 0;
can_check = 0;

CANDATA_T globalCanData[MAX_ID_NUM];



int main()
 {
//    while(1);

    Sysctl_Clock();
    can_init(1000,1);     //Debugging
//    can_init(500,1);      //Real car

    delay_ms(500);
    gpioinit(); //
//    LCD_OnOff(1);
    //when go to car must check can_to_spi_transmission, etc

    delay_ms(500);

    uart_7_init();
    spi_init_conf();
    hib_init();

    SPI_LCD_ACTIVE();
    FT81x_Init();
    ClearScreen();

    calibrate_once();

    WakePin_Init();

    //PRI10.bit7:5 <= 7 //Loweast, CAN lowest
    //PRI5.bit31:29 <=6 //Lower TIMER2 IRQ.


    Flash_read( Engine_distance_data,  4, 0);
    Flash_read( Mission_distance_data,  4, 256);
    Flash_read( Break_distance_data,  4, 512);
// ji

    memcpy(&Engine_interval, Engine_distance_data, sizeof(Engine_interval));
    memcpy(&Mission_interval, Mission_distance_data, sizeof(Mission_interval));
    memcpy(&Break_interval, Break_distance_data, sizeof(Break_interval));
    //copy distance to distance_data

    memset (globalCanData, 0, sizeof (globalCanData));  //Initialization;

    CANDATA_T localCanData[MAX_ID_NUM];


  //Real car
//    can_rx2(RPM_CAN_ID,1);
//    //*(short*)(&(localCanData[0].CANDATA[i])) = DEBUG_RPM_CAN_ID
//    can_rx2(TRANSMISSION_CAN_ID,2);
//    //*(short*)(&(localCanData[1].CANDATA[i])) = DEBUG_TRANSMISSION_CAN_ID
//    can_rx2(ACCELERATION_CAN_ID,3);
//    //*(short*)(&(localCanData[2].CANDATA[i])) = DEBUG_ACCELERATION_CAN_ID

    //(*(short *)(&(globalCanData[i].CANDATA[0])) ) = CAN1_IF1DA1_R;
    //(*(short *)(&(globalCanData[i].CANDATA[2])) ) = CAN1_IF1DA2_R;

  // Debugging
    can_rx2(DEBUG_RPM_CAN_ID,1);
    can_rx2(DEBUG_TRANSMISSION_CAN_ID,2);
    can_rx2(DEBUG_ACCELERATION_CAN_ID,3);


    //MsgID entered->inter CanRX2->IRQhandler Jump

    //memcpy(copyarr, 0xf54, 400);

    Timer0_init(0x00030d40);    //timer0 is for distance
                                //Interrupt Request every 0.1s
                                // 0x00f42400 == 16000000 == 0.2s irq
                                // 0x007a1200 == 8000000  == 0.1s irq
                                //20MHz -> 2000000로 해야 0.1s. => (jiwon) = 0x001E8480
                                // 20MHz -> 200000로 하면 0.01s => ecu에서 보내는 주기가 100Hz니까.
                                //(jiwon) = 0x0003\0D40
                                // 130000으로 해야 0.01초가 된다. 0x0001FBD0

    Timer2_init(0x01312D00);    //timer2 is for hib condition check
                                //Interrupt Request every 1s
                                // 0x04c4b400 == 160000000 == 1s irq
    //uart_7_tx_str("start\r\n");

    char *Gear,*Rpm,*Speed;
    uint8_t pressed = 0;
    uint8_t point_display = 1;
    uint8_t rst_flag = 0;
    uint8_t yes_flag = 0;

    while(1)
    {
        delay_ms(100);
        uint8_t Tag = spi_read_8(REG_TOUCH_TAG + RAM_REG);  //Check for touches
        //uart_7_tx(Tag+'0');
        ascii_Engine_distance_array();
        ascii_Mission_distance_array();
        ascii_Break_distance_array();

        NVIC_EN1_R = 0;         //IRQ disable
        memcpy(localCanData, globalCanData, sizeof (localCanData));
        for (i=0;i<MAX_ID_NUM;i++)  globalCanData[i].ifValid = 0;
        NVIC_EN1_R = 1<<8;      //IRQ enable

        if (localCanData[0].ifValid == 0)
        {
            can_to_spi_rpm(&Rpm,0);  //RPM_CAN_ID
            can_to_spi_transmission(&Gear,0);  //TRANSMISSION_CAN_ID
            can_to_spi_acceleration(&Speed,0);  //ACCELERATION_CAN_ID
        }
        else
        {   //Valid
            can_to_spi_rpm(&Rpm,*(short*)(&(localCanData[0].CANDATA[2])));  //RPM_CAN_ID
            //*(short*)(&(localCanData[0].CANDATA[i])) = DEBUG_RPM_CAN_ID
            can_to_spi_transmission(&Gear,*(short*)(&(localCanData[1].CANDATA[0])));  //TRANSMISSION_CAN_ID
            //*(short*)(&(localCanData[1].CANDATA[i])) = DEBUG_TRANSMISSION_CAN_ID
            can_to_spi_acceleration(&Speed,*(short*)(&(localCanData[2].CANDATA[2])));  //ACCELERATION_CAN_ID
            //*(short*)(&(localCanData[2].CANDATA[i])) = DEBUG_ACCELERATION_CAN_ID
        }

        if(point_display == 0)
            display_accel_trans_rpm(20,Gear,Rpm,Speed);
        else if(point_display == 1)
            display_fuel(20, Engine_distance_array, Mission_distance_array,
                         Break_distance_array);
        else if(point_display == 2 )
            display_fuel_reset(20 ,Engine_distance_array, Mission_distance_array,
                               Break_distance_array);
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
                memcpy(&Engine_interval, Engine_distance_data, sizeof(Engine_interval));
            }
            else if(rst_flag == 2)
            {
                make_character(0,"Mission oil reset ok!");
                Flash_erase(64,65);
                Flash_write(distance_data_clear,4, 256);
                Flash_read( Mission_distance_data,  4, 256);
                memcpy(&Mission_interval, Mission_distance_data, sizeof(Mission_interval));
            }
            else if(rst_flag == 3)
            {
                make_character(0,"Break oil reset ok!");
                Flash_erase(128,129);
                Flash_write(distance_data_clear,4, 512);
                Flash_read( Break_distance_data,  4, 512);
                memcpy(&Break_interval, Break_distance_data, sizeof(Break_interval));
            }
            delay_ms(100);
            yes_flag =0;
            rst_flag =0;
            point_display = 1;
        }

        if(hib_count  == 10)
        {
            //uart_7_tx_str("Hibernate\r\n");
            make_character(0,"Into Hibernation!!");
            delay_ms(1000);
            into_hibernate(1);
        }

        if((flash_count == 5))
        {
            //uart_7_tx_str("Data save\r\n");
            Flash_save();
            flash_count = 0;
        }
        if((GPIO_PORTF_DATA_R & 0x8) == 0)
        {
            flash_count = 0;
            hib_count = 0;
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




