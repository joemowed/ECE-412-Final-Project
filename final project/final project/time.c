/*
 * time.c
 *
 * Created: 4/24/2025 1:14:17 PM
 *  Author: malon
 */ 

#include "delay.h"
#include "time.h"
#include <avr/interrupt.h>
#include <stdbool.h>

#define GLOBAL_TIM_CTRLA TCCR4A
#define GLOBAL_TIM_CTRLB TCCR4B
#define GLOBAL_TIM_TCNTH TCNT4H
#define GLOBAL_TIM_TCNTL TCNT4L
#define GLOBAL_TIM_OCRAH OCR4AH
#define GLOBAL_TIM_OCRAL OCR4AL
#define GLOBAL_TIM_ISRMSK TIMSK4
#define GLOBAL_TIM_ISR_HANDLE TIMER4_COMPA_vect

//set to true by the initialization routine
bool is_global_timer_init = false;

//the start time for the quickdraw timer
unsigned long quickdraw_start_ms = 0;

// the following flags are changed by the ISR
volatile unsigned long global_ms_elapsed =0 ;

ISR(GLOBAL_TIM_ISR_HANDLE, ISR_BLOCK) {
	global_ms_elapsed++;
}


// writes zeros to the timer count register
inline void globalResetCount() {
    GLOBAL_TIM_TCNTH = 0x0;
    GLOBAL_TIM_TCNTL = 0x0;
}

// writes "value" to 16-bit timer compare A register
//  required by datasheet to write high byte first
inline void globalWriteTimerCompare(uint16_t value) {
    // store current sreg value to be restored later
    uint8_t sreg = SREG;
    // disable interrupts, reccomended by datasheet to make timer 16 bit
    // register writes atomic
    cli();
    // extract the lower 8 bits
    const volatile uint8_t low = (uint8_t)(value & 0xFF);
    // extract the upper 8 bits
    const volatile uint8_t high = (uint8_t)(value >> 8);
    GLOBAL_TIM_OCRAH = high; // write high byte
    GLOBAL_TIM_OCRAL = low;  // write low byte
    SREG = sreg;        // restore the sreg
}

// resets the timer,writes the compare value to timer compare register A and
// enables the timer interrupt for compare match A
inline void globalSetupTimerForDelay(uint16_t compare) {
	GLOBAL_TIM_CTRLA = 0x0; // reset state
	globalResetCount(); // timer counter not gaurenteed to be zero at start of delay
	globalWriteTimerCompare(compare);   // write the compare A register
	GLOBAL_TIM_ISRMSK = (1 << OCIE1A); // enable only the compare A match interrupt
}

inline void globalStartTimer() {
	// 0x1 sets clk src to cpu clk and starts timer.
	// CTC mode auto-resets the count on compareA match
	GLOBAL_TIM_CTRLB = ((0x1 << WGM12) | (0x1));
}

// stops the timer by setting clock source to none
// disables timer interrupts
inline void globalStopTimer() {
	GLOBAL_TIM_CTRLB = 0x0;
	GLOBAL_TIM_ISRMSK = 0x0;
}




// initializes and starts the timer
void globalTimerInit(const uint16_t microseconds) {
	// takes 0.313us, calculates the timer counter register value
	// given a number of microseconds
	const uint16_t count = microsecondsToPeriodCount(microseconds);
	// takes 1.8us, configures the timer to wait for the given number of
	// microseconds
	globalSetupTimerForDelay(count);
	// enable global interrupts
	sei();
	globalStartTimer();
	is_global_timer_init = true;
}

void timeStart(){
	globalTimerInit(1000);	
}

unsigned long getTime(){
	return global_ms_elapsed;
}
bool isGlobalTimerInit(){
	return is_global_timer_init;
}


/* starts counting milliseconds for the quickdraw timer.  Resets the current
 start time for the quickdraw timer if required.*/
void startDrawTimer(){
	if(!isGlobalTimerInit())
	{
	//start the global timer, if it is not already	
		timeStart();
	}
	quickdraw_start_ms = getTime();
}

/* Returns the number of milliseconds elapsed since the startDrawTimer has been called.*/
unsigned long stopDrawTimer(){
	return getTime() - quickdraw_start_ms;
}
