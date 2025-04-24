/*
 * delay.c
 *
 * Created: 3/8/2025 12:23:33 AM
 *  Author: Joe Maloney
 */
#include "delay.h"
#include <avr/interrupt.h>
#include <stdbool.h>

// the following flags are changed by the ISR

volatile bool waitFlagForDelay = false; // blocking delay flag

// global reference to user flag
volatile bool *delayUserFlag = &waitFlagForDelay;

volatile bool userFlagInUse = false; // state of the user flag

// The time it takes for delayMicroseconds to operate, if the interrupt
// triggers instantly
const uint16_t tuningMicroseconds = 9;

// register the ISR, use ISR_BLOCK to disable interrupts while
// this ISR is executing
ISR(DELAY_ISR_HANDLE, ISR_BLOCK) {
    DELAY_CTRLB = 0x0;
    // set the flag for blocking delayMicroseconds
    waitFlagForDelay = true;
    // set the user provided delay flag
    *delayUserFlag = true;
    // keep *delayUserFlag defined when not in use, and don't overwrite the
    // provided user flag more than once per call to delayFlag()
    delayUserFlag = &waitFlagForDelay;
    // mark the user flag as free
    userFlagInUse = false;
}


// writes "value" to 16-bit timer compare A register
//  required by datasheet to write high byte first
inline void writeTimerCompare(uint16_t value) {
    // store current sreg value to be restored later
    uint8_t sreg = SREG;
    // disable interrupts, reccomended by datasheet to make timer 16 bit
    // register writes atomic
    cli();
    // extract the lower 8 bits
    const volatile uint8_t low = (uint8_t)(value & 0xFF);
    // extract the upper 8 bits
    const volatile uint8_t high = (uint8_t)(value >> 8);
    DELAY_OCRAH = high; // write high byte
    DELAY_OCRAL = low;  // write low byte
    SREG = sreg;        // restore the sreg
}

// writes zeros to the timer count register
inline void resetCount() {
    DELAY_TCNTH = 0x0;
    DELAY_TCNTL = 0x0;
}

// resets the timer,writes the compare value to timer compare register A and
// enables the timer interrupt for compare match A
inline void setupTimerForDelay(uint16_t compare) {
    DELAY_CTRLA = 0x0; // reset state
    resetCount(); // timer counter not gaurenteed to be zero at start of delay
    writeTimerCompare(compare);   // write the compare A register
    DELAY_ISRMSK = (1 << OCIE1A); // enable only the compare A match interrupt
}

// calculates the value to store in timer compare register
// takes into account the overhead of configuring the timer
inline uint16_t microsecondsToPeriodCount(uint16_t value) {
    if (value > 4090) { //~4090 is max for 16bit timer at 16mhz
        return UINT16_MAX;
    }
    // works because 16mhz has 1/16 microsecond period, so multiplying "value"
    // by 16 will provide a compare match at "value" microseconds.  We also
    // subtract the call overhead from configuring the timer (a constant).
    return ((value << 4) - ((tuningMicroseconds) << 4));
}

inline void startTimer() {
    // 0x1 sets clk src to cpu clk and starts timer.
    // CTC mode auto-resets the count on compareA match
    DELAY_CTRLB = ((0x1 << WGM12) | (0x1));
}

// stops the timer by setting clock source to none
// disables timer interrupts
inline void stopTimer() {
    DELAY_CTRLB = 0x0;
    DELAY_ISRMSK = 0x0;
}

// initializes the timer
uint8_t timerInit(const uint16_t microseconds) {
    // takes 1.12us, stops the timer if it isn't already
    stopTimer();
    // takes 0.313us, calculates the timer counter register value
    // given a number of microseconds
    const uint16_t count = microsecondsToPeriodCount(microseconds);
    // takes 1.8us, configures the timer to wait for the given number of
    // microseconds
    setupTimerForDelay(count);
    // store the starting SREG to be restored later
    const uint8_t sreg = SREG;
    // enable global interrupts
    sei();
    startTimer();
    return sreg;
}

// approximates small delays. Overestimates (approxLoop(10) will result in
// ~12.5us of delay but is more accurate for smaller values)
void approxLoop(const uint16_t microseconds) {
    for (volatile uint8_t i = 0; i < microseconds - 3; i++) {
        // block for some number of iterations
    }
}

// configures the timer and blocks untill the interrupt happens
void delayWrapper(const uint16_t microseconds) {
    uint8_t sreg = timerInit(microseconds); // configure and start timer
    while (!waitFlagForDelay) {
        // block untill interrupt occurs
    }
    waitFlagForDelay = false;
    stopTimer(); // stop the timer
    SREG = sreg; // restore the SREG
}
/* Sets the given flag to to true when the given number of microseconds has
 * passed.  This function needs to be polled.*/
void delayFlag(volatile bool *flag, const uint16_t microseconds) {
    if (userFlagInUse) {
        // do nothing if already waiting for an interrupt to set the user flag
        return;
    }
    userFlagInUse = true; // store state as waiting for user flag interrupt
    delayUserFlag = flag; // store user flag address as global for ISR
    if (microseconds < tuningMicroseconds) {
        // skip the call to configure and start the timer if the delay is very
        // small, as configuring and starting the timer has ~11us of overhead
        *flag = true;
    }
    // start the timer and enable the interrupt
    timerInit(microseconds);
}

/* This function is broken up into other functions in order to facilitate lower
 * delay times.  Without seperating the appoxLoop and delayWrapper functions,
 * the function call to this function takes ~3us.  Seperating them reduces the
 * function call overhead enabling a 1us delay.*/
void delayMicroseconds(const uint16_t microseconds) {
    if (microseconds <= 2) { // function call alone takes 938 nanoseconds
        return;
    } else {
        if (microseconds < tuningMicroseconds) {
            // for small delays, approximate with a loop because using an
            // interrupt based timer takes ~11us, in setting up the timer
            approxLoop(microseconds);
        } else {
            // for larger delays, use the interrupt based timer for its accuracy
            delayWrapper(microseconds);
        }
    }
}
