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

#include <header/debugging_h.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


//#define hundread_thousand 100000
//#define ten_thousand 10000
//#define thousand 1000
//#define hundread 100
//#define ten 10
//#define one 1

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

//extern char distance_array[16], Engine_distance_array[16], Mission_distance_array[16], Break_distance_array[16];
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

extern uint32_t GetTickCheck;

void Timer0_init(uint32_t period)
{
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

void Timer3_init(uint32_t period)
{
    SYSCTL_RCGCTIMER_R |= 0x08; // Timer3
    TIMER3_CTL_R &= ~0x01;  // disable Timer3A during setup
    TIMER3_CFG_R = 0x00; // 32-bit mode
    TIMER3_TAMR_R = 0x02; // Periodic timer mode
    TIMER3_TAILR_R = period-1; // reload value
    TIMER3_TAPR_R = 0; // prescale (0: 1)
    TIMER3_ICR_R = 0x01; // TATOCINT, clear timer3A Time out interrupt
    TIMER3_IMR_R = 0x01; // TATOIM, Time-Out Int. Mask

    // Timer 3A vector number: 51, Interrupt number:23
    NVIC_PRI8_R = 0x00000000; // pri 4
    NVIC_EN1_R |= 0x08; // IRQ

    TIMER3_CTL_R = 0x01; // Enable Timer2A.
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

void timer2_handler(void)   //check gear, speed, rpm and enter hibernate ISR
{
    TIMER2_ICR_R = TIMER_ICR_TATOCINT; //clear the interrupt request
    CANDATA_T isr2CanData[MAX_ID_NUM];

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
        hib_count = 0;
    }
#if (debugging == 1)
    sec0++;
    if( (sec0%3600) == 0)
    {
        uart_7_tx_str("e\r\n");
    }
#endif
}

void timer3_handler(void)
{
    TIMER3_ICR_R = TIMER_ICR_TATOCINT;
    GetTickCheck++;
}

