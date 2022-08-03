/*
 * timer.c
 *
 *  Created on: 2021. 5. 3.
 *      Author: kjm-hsh
 */

#include <header/Can_init.h>
#include <header/Gpio_led.h>
#include <header/Uart.h>
#include <header/Uart7.h>


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

// xx
extern int hib_flag, hib_count, flash_count, timer2_flag, timercheck_count, timercheck_count2;
extern volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
extern volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
extern char distance_array[16], Engine_distance_array[16], Mission_distance_array[16], Break_distance_array[16];
extern int *speed_ptr;
int sec0 = 0;

// xx

// yy
extern volatile double Engine_interval, Mission_interval, Break_interval;
extern volatile double Engine_distance2, Mission_distance2, Break_distance2;

// yy

extern char distance_array[16], Engine_distance_array[16], Mission_distance_array[16], Break_distance_array[16];
extern uint32_t distance_data[2];

extern uint32_t Engine_distance_data[sizeof(Engine_distance)];
extern uint32_t Mission_distance_data[sizeof(Mission_distance)];
extern uint32_t Break_distance_data[sizeof(Break_distance)];

/*//yy
extern uint32_t Engine_distance_data[sizeof(Engine_distance2)];
extern uint32_t Mission_distance_data[sizeof(Mission_distance2)];
extern uint32_t Break_distance_data[sizeof(Break_distance2)];
//tt */
extern volatile uint32_t isr_flag ;

void Timer0_init(uint32_t period)
{
    //TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0 ;
    //Enable timer0 clock gating
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //Disable before making change
    TIMER0_CFG_R =0 ;
    //For a 16/32-bit timer, this value selects the 32-bit timer configuration
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;
    //Periodic Timer mode
    TIMER0_TAILR_R = period-1;
    //Load the start value
    TIMER0_TAPR_R = 0;
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
    //Writing a 1 to this bit clears the TATORIS bit in the GPTMRIS register
    //and the TATOMIS bit in the GPTMMIS register.
    TIMER0_IMR_R |= TIMER_IMR_TATOIM;
    //Interrupt mask
    NVIC_PRI4_R = 0x00000000;
    //irq
    NVIC_EN0_R =  1 << 19;
    //irq
    TIMER0_CTL_R = TIMER_CTL_TAEN;
    //Start counting
}

void Timer2_init(uint32_t period)
{
    //TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

    SYSCTL_RCGCTIMER_R |= 0x04; // Timer2
    TIMER2_CTL_R &= ~0x01;  // disable Timer2A during setup
    TIMER2_CFG_R = 0x00; // 32-bit mode
    TIMER2_TAMR_R = 0x02; // Periodic timer mode
    TIMER2_TAILR_R = period-1; // reload value
    TIMER2_TAPR_R = 0; // prescale (0: 1)
    TIMER2_ICR_R = 0x01; // TATOCINT, clear timer2A Time out interrupt
    TIMER2_IMR_R = 0x01; // TATOIM, Time-Out Int. Mask

    // Timer 2A vector number: 39, Interrupt number:23
    NVIC_PRI5_R = 0xC0000000; // pri 4
    NVIC_EN0_R = 1<<23; // IRQ

    TIMER2_CTL_R = 0x01; // Enable Timer2A.
}

void Timer0_disable()
{
    TIMER0_CTL_R &= ~0x01;  // disable Timer2A during setup
}

void Timer2_disable()
{
    TIMER2_CTL_R &= ~0x01;  // disable Timer2A during setup
}

void Timer0_clear_irq(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
}

void timer0_handler(void)   //convert receive can data to char ptr for display ISR
{

    //LED_OnOff(1);
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
        uint32_t a2_1=*(short*)(&(isr1CanData[2].CANDATA[2])); // real car
        uint32_t a2_2= (uint32_t)((a2_1>>8)&0x000000ff);// debugging

        static char DD1[2]={0,};
        // Real car
//                DD1[0]=((char)a2_1>>4);     //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> >>4 == 0x0012
//                DD1[1]=((char)a2_1 & 0x0f); //a2_1 == 0x0000.1234 -> char a2_1 == 0x1234 -> &0x0f == 0x0034


        // Debugging
        DD1[0]=((char)a2_2>>4);     //a2_2 == 0x0000.1234 -> char a2_2 == 0x1234 -> >>4 == 0x0012
        DD1[1]=((char)a2_2 & 0x0f); //a2_2 == 0x0000.1234 -> char a2_2 == 0x1234 -> &0x0f == 0x0034


        speed=DD1[0]*16+DD1[1]; //hex to decimal
        // 경계선 //

//        Engine_interval += speed;
//        Mission_interval += speed;
//        Break_interval += speed;
        Engine_interval = speed + Engine_interval;
        Mission_interval = speed + Mission_interval;
        Break_interval = speed + Break_interval;


        Engine_distance2 = Engine_interval * 2.7778e-4;
        Mission_distance2 = Mission_interval * 2.7778e-5;
        Break_distance2 = Break_interval * 2.7778e-6;
    }
    //flash에 저장할때 필요한 변수
    memcpy(Engine_distance_data,&Engine_interval,sizeof(Engine_distance_data));
    memcpy(Mission_distance_data,&Mission_interval,sizeof(Mission_distance_data));
    memcpy(Break_distance_data,&Break_interval,sizeof(Break_distance_data));



    // 거리 계산에 필요한 변수
    Engine_int_distance = (int)Engine_distance2;
    Mission_int_distance = (int)Mission_distance2;
    Break_int_distance= (int)Break_distance2;

    //LED_OnOff(2);
    //    GPIO_PORTD_DEN_R     = (GPIO_PORTD_DEN_R &(~(unsigned long)0xFF))| 0x01;
}

void timer2_handler(void)   //check gear, speed, rpm and enter hibernate ISR
{
    TIMER2_ICR_R = TIMER_ICR_TATOCINT; //clear the interrupt request
    CANDATA_T isr2CanData[MAX_ID_NUM];
    char secarr[10] = {0, };

    int i;
    NVIC_EN1_R = 0;       //IRQ disable
    memcpy(isr2CanData, globalCanData, sizeof (isr2CanData));
    for (i=0;i<MAX_ID_NUM;i++)  globalCanData[i].ifValid = 0;
    NVIC_EN1_R = 1<<8;    //IRQ enable

    uint32_t a1_1=*(short*)(&(isr2CanData[1].CANDATA[0]));

    uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01
    static char DC1[2]={0,};
    DC1[0]=((char)a1_2>>4);
    DC1[1]=((char)a1_2 & 0x0f);

    if(DC1[1] == 0)
    {
        flash_count++;
        hib_count++;
    }
    else
    {
        flash_count=0;
        hib_count = 0;
    }
//    sec0++;
//    if( (sec0%3600) == 0)
//    {
//        uart_7_tx_str("H\r\n");
//    }
}





void ascii_distance_array(void)
{
    /////////////////////////////////////////////0//////100000///////////////////////////////////////////////////////////
    if (
            (int_distance / hundread_thousand)==0
    )
        distance_array[0] = 0x20;

    else distance_array[0] = int_distance / hundread_thousand +'0';
    /////////////////////////////////////////////1//////010000///////////////////////////////////////////////////////////
    //if ((int_distance % hundread_thousand) / ten_thousand ==0)  distance_array[1] = 0x20;
    if
    (
            (int_distance / hundread_thousand == 0)   &&
            ((int_distance % hundread_thousand) / ten_thousand ==0)
    )    distance_array[1] = 0x20;
    /*
            else if
            (      ((int_distance % hundread_thousand) / ten_thousand == 0) &&
                    ((int_distance / hundread_thousand >0)     )
            )   distance_array[1]='0';
     */
    else distance_array[1] = (int_distance % hundread_thousand) / ten_thousand +'0';
    /////////////////////////////////////////////2//////001000///////////////////////////////////////////////////////////
    //if (((int_distance % hundread_thousand) % ten_thousand) / thousand ==0)  distance_array[2] = 0x20;
    if
    (
            (int_distance / hundread_thousand == 0) &&
            ((int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((int_distance % hundread_thousand) % ten_thousand) / thousand  ==0)
    )    distance_array[2] = 0x20;
    /*
            else if
            (      (((int_distance % hundread_thousand) % ten_thousand) / thousand == 0) &&
                    (((int_distance % hundread_thousand) / ten_thousand  >0)     )
            )   distance_array[2]='0';
     */
    else distance_array[2] = ((int_distance % hundread_thousand) % ten_thousand ) / thousand+'0';

    /////////////////////////////////////////////3//////000100///////////////////////////////////////////////////////////
    //    if ((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread==0)  distance_array[3] = 0x20;
    if
    (
            (int_distance / hundread_thousand == 0) &&
            ((int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)
    )    distance_array[3] = 0x20;
    /*
        else if
        (      (((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread)== 0) &&
                (((((int_distance % hundread_thousand) % ten_thousand) / thousand )  >0)     )
        )    distance_array[3]='0';
     */
    else distance_array[3] = (((int_distance % hundread_thousand) % ten_thousand ) % thousand) /hundread +'0';
    /////////////////////////////////////////////4//////000010///////////////////////////////////////////////////////////
    if (
            (int_distance / hundread_thousand == 0) &&
            ((int_distance % hundread_thousand) / ten_thousand ==0) &&
            (((int_distance % hundread_thousand) % ten_thousand) / thousand  ==0) &&
            ((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread ==0)   &&
            (((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)
    )
        distance_array[4] = 0x20;
    /*
        else if
        (
                (((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten==0)  &&
                ((((int_distance % hundread_thousand) % ten_thousand) % thousand ) /hundread > 0)
        )
                distance_array[4] = '0';
     */
    else distance_array[4] = ((((int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) /ten+'0';

    /////////////////////////////////////////////5//////000001///////////////////////////////////////////////////////////
    /*
        if
        ((((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one==0)
            distance_array[5] = '0';

        else if
        (      ((((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) %ten) /one== 0) &&
                (((((int_distance % hundread_thousand) % ten_thousand) % thousand ) %hundread) /ten >0)     )
            distance_array[5]='0';
     */
    distance_array[5] = (((((int_distance % hundread_thousand) % ten_thousand ) % thousand) %hundread) %ten) /one+'0';

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
