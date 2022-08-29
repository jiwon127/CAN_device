/*
 * FlashtoRAMtest.c
 *
 *  Created on: 2022. 8. 11.
 *      Author: esllab
 */

#pragma SRC

#include <header/Can_init.h>
#include <header/Gpio_led.h>
#include <header/Uart.h>
#include <header/Uart7.h>

#include <header/debugging_h.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


extern volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
extern volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km

extern volatile double Engine_interval, Mission_interval, Break_interval;
extern volatile double Engine_distance2, Mission_distance2, Break_distance2;


extern uint32_t Engine_distance_data[sizeof(Engine_distance)];
extern uint32_t Mission_distance_data[sizeof(Mission_distance)];
extern uint32_t Break_distance_data[sizeof(Break_distance)];


void timer0test_handler(void)   //convert receive can data to char ptr for display ISR
{
    //    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x03;
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; //clear the interrupt request

    CANDATA_T isr1CanData[MAX_ID_NUM];
    int i;

    NVIC_EN1_R = 0;         //IRQ disable
    memcpy(isr1CanData, globalCanData, sizeof (isr1CanData));
    for (i=0;i<MAX_ID_NUM;i++)  globalCanData[i].ifValid = 0;
    NVIC_EN1_R = 1<<8;      //IRQ enable

    if (isr1CanData[0].ifValid == 0)
    {
        //uart_7_tx_str("In tim0 irq no packet from can\r\n");
        ;
    }
    else
    {
        //LED_OnOff(1);
#if (debugging == 0)
        uint32_t a2_1=*(short*)(&(isr1CanData[2].CANDATA[2])); // real car
#endif

#if (debugging == 1)
//        uint32_t a2_1=*(short*)(&(isr1CanData[2].CANDATA[0]));   // debugging
        uint32_t a2_1=*(short*)(&(isr1CanData[2].CANDATA[2])); // real car
        uint32_t a2_2= (uint32_t)((a2_1>>8)&0x000000ff);// debugging
#endif
        static char DD1[2]={0,};

#if (debugging == 0)
        //         Real car
        DD1[0]=((char)a2_1>>4);     //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> >>4 == 0x0012
        DD1[1]=((char)a2_1 & 0x0f); //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> &0x0f == 0x0034
#endif

#if (debugging ==1 )
        DD1[0]=((char)a2_2>>4);     //a2_2 == 0x0000.1234 -> char a2_2 == 0x1234 -> >>4 == 0x0012
        DD1[1]=((char)a2_2 & 0x0f); //a2_2 == 0x0000.1234 -> char a2_2 == 0x1234 -> &0x0f == 0x0034
#endif

        speed=DD1[0]*16+DD1[1]; //hex to decimal

        Engine_interval += speed;
        Mission_interval += speed;
        Break_interval += speed;

        Engine_distance2 = Engine_interval * 2.7778e-5;
        Mission_distance2 = Mission_interval * 2.7778e-5;
        Break_distance2 = Break_interval * 2.7778e-5;
    }
    //flash
    memcpy(Engine_distance_data,&Engine_interval,sizeof(Engine_distance_data));
    memcpy(Mission_distance_data,&Mission_interval,sizeof(Mission_distance_data));
    memcpy(Break_distance_data,&Break_interval,sizeof(Break_distance_data));

    // �뜝�떊紐뚯삕 �뜝�룞�삕饔낉옙 �뜝�떗�슱�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
    Engine_int_distance = (int)Engine_distance2;
    Mission_int_distance = (int)Mission_distance2;
    Break_int_distance= (int)Break_distance2;

    //LED_OnOff(2);
    //    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x01;
}
