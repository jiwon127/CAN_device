/*
 * timer.h
 *
 *  Created on: 2021. 5. 3.
 *      Author: kjm-hsh
 */
#include <stdint.h>

#ifndef HEADER_TIMER_H_
#define HEADER_TIMER_H_

void Timer0_init(uint32_t period);
void Timer0_disable();
void Timer0_clear_irq(void);
void timer0_handler(void);

void Timer2_init(uint32_t period);
void Timer2_disable();
void timer2_handler(void);

void Timer3_init(uint32_t period);
void timer3_handler(void);

void init_interval(uint8_t data);

//void ascii_Engine_distance_array(void);
//void ascii_Mission_distance_array(void);
//void ascii_Break_distance_array(void);

#endif /* HEADER_TIMER_H_ */
