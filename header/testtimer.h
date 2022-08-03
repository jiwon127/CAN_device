/*
 * testtimer.h
 *
 *  Created on: 2022. 5. 16.
 *      Author: esllab
 */
#include <stdint.h>

#ifndef HEADER_TESTTIMER_H_
#define HEADER_TESTTIMER_H_


void Timer0_init(uint32_t period);
void Timer0_disable();
void Timer0_clear_irq(void);
void timer0_handler(void);


#endif /* HEADER_TESTTIMER_H_ */
