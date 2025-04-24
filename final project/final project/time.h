/*
 * time.h
 *
 * Created: 4/24/2025 1:45:20 PM
 *  Author: malon
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdbool.h>

// starts the global timer uses TIM4
void timeStart();

// returns the number of milliseconds since the global timer has started
unsigned long getTime();

//returns true if the global timer is counting
bool isGlobalTimerInit();

/* starts counting milliseconds for the quickdraw timer.  Resets the current
 start time for the quickdraw timer if required.*/
void startDrawTimer();

/* Returns the number of milliseconds elapsed since the startDrawTimer has been called.*/
unsigned long stopDrawTimer();

#endif /* TIME_H_ */