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
volatile unsigned int IRQ_A1,IRQ_A2, IRQ_B1, IRQ_B2;
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
//void can_init2(int irq);
//void can_rx2(int id, char msgBoxNum);

int main(void)
{
    memset (globalCanData, 0, sizeof (globalCanData));  //Initialization;
    CANDATA_T localCanData[MAX_ID_NUM];
    Sysctl_Clock();
    uart_7_init();
    can_init(1000,1);           //Interrupt Setup (IE enable)
    can_rx2(0x3b,1);         //MsgID entered->inter CanRX2->IRQhandler Jump
    can_rx2(0x3cb,2);         //MsgID entered->inter CanRX2->IRQhandler Jump
    can_rx2(0x6b2,3);         //MsgID entered->inter CanRX2->IRQhandler Jump

    Timer0_init(0x007a1200);    //timer0 is for distance

    //delay_ms(1000);
    while(1)
    {
        //IRQ disable
        NVIC_EN1_R = 0; // IRQ
        memcpy(localCanData, globalCanData, sizeof (localCanData));
        for (i=0;i<MAX_ID_NUM;i++)  globalCanData[i].ifValid = 0;
        //IRQ enable
        NVIC_EN1_R = 1<<8; // IRQ


        if (localCanData[0].ifValid == 0)
        {
            uart_7_tx_str("WROTN!!!R!R!R!R!R nO packet from CAN!\r\n");
        }
        else
        {   //Valid
            //printReceivedCanData(7, *(short*)(&(localCanData[0].CANDATA[0])), *(short*)(&(localCanData[0].CANDATA[2])), *(short*)(&(localCanData[0].CANDATA[4])), *(short*)(&(localCanData[0].CANDATA[6])));
            //printReceivedCanData(7, *(short*)(&(localCanData[1].CANDATA[0])), *(short*)(&(localCanData[1].CANDATA[2])), *(short*)(&(localCanData[1].CANDATA[4])), *(short*)(&(localCanData[1].CANDATA[6])));
            //printReceivedCanData(7, *(short*)(&(localCanData[2].CANDATA[0])), *(short*)(&(localCanData[2].CANDATA[2])), *(short*)(&(localCanData[2].CANDATA[4])), *(short*)(&(localCanData[2].CANDATA[6])));
            uart_7_tx('\n');
        }
        delay_ms(100);
    }
    return 0;
}

