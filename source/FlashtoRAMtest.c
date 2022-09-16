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

#define hundread_thousand 100000
#define ten_thousand 10000
#define thousand 1000
#define hundread 100
#define ten 10
#define one 1


extern volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
extern volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km

extern volatile double Engine_interval, Mission_interval, Break_interval;
extern volatile double Engine_distance2, Mission_distance2, Break_distance2;


extern uint32_t Engine_distance_data[sizeof(Engine_distance)];
extern uint32_t Mission_distance_data[sizeof(Mission_distance)];
extern uint32_t Break_distance_data[sizeof(Break_distance)];

extern char distance_array[16], Engine_distance_array[16], Mission_distance_array[16], Break_distance_array[16];



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

        Engine_distance2 = Engine_interval * 2.7778e-6;
        Mission_distance2 = Mission_interval * 2.7778e-6;
        Break_distance2 = Break_interval * 2.7778e-6;
    }
    //flash
    memcpy(Engine_distance_data,&Engine_interval,sizeof(Engine_distance_data));
    memcpy(Mission_distance_data,&Mission_interval,sizeof(Mission_distance_data));
    memcpy(Break_distance_data,&Break_interval,sizeof(Break_distance_data));

    // 占쎈쐻占쎈뻿筌뤿슣�굲 占쎈쐻占쎈짗占쎌굲耀붾굢�삕 占쎈쐻占쎈뼏占쎌뒻占쎌굲占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈짗占쎌굲
    Engine_int_distance = (int)Engine_distance2;
    Mission_int_distance = (int)Mission_distance2;
    Break_int_distance= (int)Break_distance2;

    //LED_OnOff(2);
    //    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x01;
}

void ascii_Engine_distance_array(void)
{
    memset(Engine_distance_array,0,sizeof(Engine_distance_array));
    /////////////////////////////////////////////0//////100000///////////////////////////////////////////////////////////
    if ( ( Engine_int_distance / hundread_thousand ) == 0 )
        Engine_distance_array[0] = 0x20;

    else Engine_distance_array[0] = Engine_int_distance / hundread_thousand +'0';

    /////////////////////////////////////////////1//////010000///////////////////////////////////////////////////////////
    //if ((Engine_int_distance % hundread_thousand) / ten_thousand ==0)  Engine_distance_data[1] = 0x20;
    if
    (
            (Engine_int_distance / hundread_thousand == 0)   &&
            ((Engine_int_distance % hundread_thousand) / ten_thousand == 0 )
    )    Engine_distance_array[1] = 0x20;
    /*
            else if
            (      ((Engine_int_distance % hundread_thousand) / ten_thousand == 0) &&
                    ((Engine_int_distance / hundread_thousand >0)     )
            )   Engine_distance_data[1]='0';
     */
    else Engine_distance_array[1] = (Engine_int_distance % hundread_thousand) / ten_thousand +'0';

    /////////////////////////////////////////////2//////001000///////////////////////////////////////////////////////////
    //if (((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand ==0)  Engine_distance_data[2] = 0x20;
    if
    (
            (Engine_int_distance / hundread_thousand == 0) &&
            ((Engine_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0)
    )    Engine_distance_array[2] = 0x20;
    /*
            else if
            (      (((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand == 0) &&
                    (((Engine_int_distance % hundread_thousand) / ten_thousand  >0)     )
            )   Engine_distance_data[2]='0';
     */
    else Engine_distance_array[2] = ((Engine_int_distance % hundread_thousand) % ten_thousand ) / thousand+'0';


    /////////////////////////////////////////////3//////000100///////////////////////////////////////////////////////////
    //    if ((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread==0)  Engine_distance_data[3] = 0x20;
    if
    (
            (Engine_int_distance / hundread_thousand == 0) &&
            ((Engine_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)
    )    Engine_distance_array[3] = 0x20;
    /*
        else if
        (      (((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread)== 0) &&
                (((((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand )  >0)     )
        )    Engine_distance_data[3]='0';
     */
    else Engine_distance_array[3] = (((Engine_int_distance % hundread_thousand) % ten_thousand ) % thousand) /hundread +'0';

    /////////////////////////////////////////////4//////000010///////////////////////////////////////////////////////////
    if (
            (Engine_int_distance / hundread_thousand == 0) &&
            ((Engine_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Engine_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)   &&
            (((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)
    )
        Engine_distance_array[4] = 0x20;
    /*
        else if
        (
                (((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)  &&
                ((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread > 0)
        )
                Engine_distance_data[4] = '0';
     */
    else Engine_distance_array[4] = ((((Engine_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) /ten+'0';


    /////////////////////////////////////////////5//////000001///////////////////////////////////////////////////////////
    /*
        if
        ((((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one==0)
            Engine_distance_data[5] = '0';

        else if
        (      ((((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one== 0) &&
                (((((Engine_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten >0)     )
            Engine_distance_data[5]='0';
     */
    Engine_distance_array[5] = (((((Engine_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) %ten) /one+'0';

}

void ascii_Mission_distance_array(void)
{
    memset(Mission_distance_array,0,sizeof(Mission_distance_array));

    /////////////////////////////////////////////0//////100000///////////////////////////////////////////////////////////
    if (
            (Mission_int_distance / hundread_thousand)==0
    )
        Mission_distance_array[0] = 0x20;

    else Mission_distance_array[0] = Mission_int_distance / hundread_thousand +'0';
    /////////////////////////////////////////////1//////010000///////////////////////////////////////////////////////////
    //if ((Mission_int_distance % hundread_thousand) / ten_thousand ==0)  Mission_distance_array[1] = 0x20;
    if
    (
            (Mission_int_distance / hundread_thousand == 0)   &&
            ((Mission_int_distance % hundread_thousand) / ten_thousand ==0)
    )    Mission_distance_array[1] = 0x20;
    /*
            else if
            (      ((Mission_int_distance % hundread_thousand) / ten_thousand == 0) &&
                    ((Mission_int_distance / hundread_thousand >0)     )
            )   Mission_distance_array[1]='0';
     */
    else Mission_distance_array[1] = (Mission_int_distance % hundread_thousand) / ten_thousand +'0';
    /////////////////////////////////////////////2//////001000///////////////////////////////////////////////////////////
    //if (((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand ==0)  Mission_distance_array[2] = 0x20;
    if
    (
            (Mission_int_distance / hundread_thousand == 0) &&
            ((Mission_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0)
    )    Mission_distance_array[2] = 0x20;
    /*
            else if
            (      (((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand == 0) &&
                    (((Mission_int_distance % hundread_thousand) / ten_thousand  >0)     )
            )   Mission_distance_array[2]='0';
     */
    else Mission_distance_array[2] = ((Mission_int_distance % hundread_thousand) % ten_thousand ) / thousand+'0';

    /////////////////////////////////////////////3//////000100///////////////////////////////////////////////////////////
    //    if ((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread==0)  Mission_distance_array[3] = 0x20;
    if
    (
            (Mission_int_distance / hundread_thousand == 0) &&
            ((Mission_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)
    )    Mission_distance_array[3] = 0x20;
    /*
        else if
        (      (((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread)== 0) &&
                (((((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand )  >0)     )
        )    Mission_distance_array[3]='0';
     */
    else Mission_distance_array[3] = (((Mission_int_distance % hundread_thousand) % ten_thousand ) % thousand) /hundread +'0';
    /////////////////////////////////////////////4//////000010///////////////////////////////////////////////////////////
    if (
            (Mission_int_distance / hundread_thousand == 0) &&
            ((Mission_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Mission_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)   &&
            (((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)
    )
        Mission_distance_array[4] = 0x20;
    /*
        else if
        (
                (((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)  &&
                ((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread > 0)
        )
                Mission_distance_array[4] = '0';
     */
    else Mission_distance_array[4] = ((((Mission_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) /ten+'0';

    /////////////////////////////////////////////5//////000001///////////////////////////////////////////////////////////
    /*
        if
        ((((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one==0)
            Mission_distance_array[5] = '0';

        else if
        (      ((((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one== 0) &&
                (((((Mission_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten >0)     )
            Mission_distance_array[5]='0';
     */
    Mission_distance_array[5] = (((((Mission_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) %ten) /one+'0';

}

void ascii_Break_distance_array(void)
{
    memset(Break_distance_array,0,sizeof(Break_distance_array));

    /////////////////////////////////////////////0//////100000///////////////////////////////////////////////////////////
    if (
            (Break_int_distance / hundread_thousand)==0
    )
        Break_distance_array[0] = 0x20;

    else Break_distance_array[0] = Break_int_distance / hundread_thousand +'0';
    /////////////////////////////////////////////1//////010000///////////////////////////////////////////////////////////
    //if ((Break_int_distance % hundread_thousand) / ten_thousand ==0)  Break_distance_array[1] = 0x20;
    if
    (
            (Break_int_distance / hundread_thousand == 0)   &&
            ((Break_int_distance % hundread_thousand) / ten_thousand ==0)
    )    Break_distance_array[1] = 0x20;
    /*
            else if
            (      ((Break_int_distance % hundread_thousand) / ten_thousand == 0) &&
                    ((Break_int_distance / hundread_thousand >0)     )
            )   Break_distance_array[1]='0';
     */
    else Break_distance_array[1] = (Break_int_distance % hundread_thousand) / ten_thousand +'0';
    /////////////////////////////////////////////2//////001000///////////////////////////////////////////////////////////
    //if (((Break_int_distance % hundread_thousand) % ten_thousand) / thousand ==0)  Break_distance_array[2] = 0x20;
    if
    (
            (Break_int_distance / hundread_thousand == 0) &&
            ((Break_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Break_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0)
    )    Break_distance_array[2] = 0x20;
    /*
            else if
            (      (((Break_int_distance % hundread_thousand) % ten_thousand) / thousand == 0) &&
                    (((Break_int_distance % hundread_thousand) / ten_thousand  >0)     )
            )   Break_distance_array[2]='0';
     */
    else Break_distance_array[2] = ((Break_int_distance % hundread_thousand) % ten_thousand ) / thousand+'0';

    /////////////////////////////////////////////3//////000100///////////////////////////////////////////////////////////
    //    if ((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread==0)  Break_distance_array[3] = 0x20;
    if
    (
            (Break_int_distance / hundread_thousand == 0) &&
            ((Break_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Break_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)
    )    Break_distance_array[3] = 0x20;
    /*
        else if
        (      (((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread)== 0) &&
                (((((Break_int_distance % hundread_thousand) % ten_thousand) / thousand )  >0)     )
        )    Break_distance_array[3]='0';
     */
    else Break_distance_array[3] = (((Break_int_distance % hundread_thousand) % ten_thousand ) % thousand) /hundread +'0';
    /////////////////////////////////////////////4//////000010///////////////////////////////////////////////////////////
    if (
            (Break_int_distance / hundread_thousand == 0) &&
            ((Break_int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((Break_int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)   &&
            (((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)
    )
        Break_distance_array[4] = 0x20;
    /*
        else if
        (
                (((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)  &&
                ((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread > 0)
        )
                Break_distance_array[4] = '0';
     */
    else Break_distance_array[4] = ((((Break_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) /ten+'0';

    /////////////////////////////////////////////5//////000001///////////////////////////////////////////////////////////

    if
    ((((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one==0)
        Break_distance_array[5] = '0';
    /*
        else if
        (      ((((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one== 0) &&
                (((((Break_int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten >0)     )
            Break_distance_array[5]='0';
     */
    else Break_distance_array[5] = (((((Break_int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) %ten) /one+'0';

}

void can_to_spi_rpm(char* (*a),unsigned int arg1)
{

    uint32_t a2_1=arg1;   //CAN1_IF1DA2_R
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //

    static char DB1[2]={0,};

    DB1[0]=((char)a2_2>>4);
    DB1[1]=((char)a2_2 & 0x0f);

    DB1[0]=DB1[0]+'0';

    static char DB2[8]={'0','0','0','0','R','P','M','\0'};
    DB2[0]=DB1[0];

    *a=DB2;
}

void can_to_spi_acceleration(char* (*a),unsigned int arg1)
{
    uint32_t a2_1=arg1;   //real car  CAN1_IF1DA2_R
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B -> debugging

    static char DD1[2]={0,};

#if (debugging == 0)
    //Real code
    DD1[0]=((char)a2_1>>4);
    DD1[1]=((char)a2_1 & 0x0f);
#endif

#if (debugging == 1)
    // Debugging
    DD1[0]=((char)a2_2>>4);
    DD1[1]=((char)a2_2 & 0x0f);
#endif

    static char DD2[8]={0,0,0,'K','m','/','h','\0'};

    int i;

    i=DD1[0]*16+DD1[1];

    if((i/100)==0) DD2[0]=0x20;       else DD2[0]=i/100 + '0';
    if((i%100)/10 ==0) DD2[1]=0x20;   else DD2[1]=(i%100)/10 + '0';

    DD2[2]=((i%100)%10) + '0';

    *a=DD2;
}

void can_to_spi_transmission(char* (*a),unsigned int arg1)
{
    uint32_t a1_1=arg1;   //CAN1_IF1DA1_R
    uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

    static char DC1[2]={0,};

    DC1[0]=((char)a1_2>>4);
    DC1[1]=((char)a1_2 & 0x0f);

    static char DC2[2]={'0','\0'};

    switch(DC1[1]) {
    case 5 :
        DC2[0]='D';
        break;
    case 6 :
        DC2[0]='N';
        break;
    case 7 :
        DC2[0]='R';
        break;
    case 0 :
        DC2[0]='P';
        break;
    default:
        DC2[0]='P';
    }

    *a=DC2;
}
