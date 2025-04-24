/*
 * timerAPI.h
 *
 * Created: 3/7/2025 12:47:00 PM
 *  Author: Joe Maloney
 */

#ifndef TIMERAPI_H_
#define TIMERAPI_H_

#include <avr/common.h>
#include <stdbool.h>
#include <stdint-gcc.h>

// select the timer to use here.  Works with TIM1,TIM3, and TIM4
#define DELAY_TIMER_NUMBER 3

#if DELAY_TIMER_NUMBER == 1 // timer register definitions for TIM1
#define DELAY_CTRLA TCCR1A
#define DELAY_CTRLB TCCR1B
#define DELAY_TCNTH TCNT1H
#define DELAY_TCNTL TCNT1L
#define DELAY_OCRAH OCR1AH
#define DELAY_OCRAL OCR1AL
#define DELAY_ISRMSK TIMSK1
#define DELAY_ISR_HANDLE TIMER1_COMPA_vect

#elif DELAY_TIMER_NUMBER == 3 // timer register definitions for TIM1
#define DELAY_CTRLA TCCR3A
#define DELAY_CTRLB TCCR3B
#define DELAY_TCNTH TCNT3H
#define DELAY_TCNTL TCNT3L
#define DELAY_OCRAH OCR3AH
#define DELAY_OCRAL OCR3AL
#define DELAY_ISRMSK TIMSK3
#define DELAY_ISR_HANDLE TIMER3_COMPA_vect

#elif DELAY_TIMER_NUMBER == 4 // timer register definitions for TIM4
#define DELAY_CTRLA TCCR4A
#define DELAY_CTRLB TCCR4B
#define DELAY_TCNTH TCNT4H
#define DELAY_TCNTL TCNT4L
#define DELAY_OCRAH OCR4AH
#define DELAY_OCRAL OCR4AL
#define DELAY_ISRMSK TIMSK4
#define DELAY_ISR_HANDLE TIMER4_COMPA_vect

#else
#error selected timer not a valid timer for delayMicroseconds
#endif

// max 4090 microseconds, min 1 microsecond. Function block program execution
// for the given number of microseconds
void delayMicroseconds(uint16_t microseconds);

// Non-blocking delay function.  Sets the given flag to true after the given
// amount of microseconds has passed. will flip flag instantly (~1us) if
// microseconds is less than 12
// Note: Calls to delayMicroseconds will reset the timer used by delayFlag() and
// flip the flag at a time other than requested by delayFlag. Do not use
// delayMicroseconds() before the flag in delayFlag() has been flipped
void delayFlag(volatile bool *flag, const uint16_t microseconds);

uint16_t microsecondsToPeriodCount(uint16_t value);

//starts the global timer uses TIM4
void timeStart();

//returns the number of milliseconds since the global timer has started
unsigned long getTime();

#endif /* TIMERAPI_H_ */
