/*
 * final project.c
 *
 * Created: 4/24/2025 10:46:21 AM
 * Author : AustJ
 */

#include <avr/io.h>

#include "LCD.h"
#include "delay.h"
#include "random.h"
#include <util/delay.h>
int main(void) { /* Replace with your application code */
#define ICR1_TOP_VALUE 2075
  LCDInit();
   LCDPuts("a string3, %i",3);
   LCDPuts2("a string3, %i",365);
   timeStart();


  while (1) {
  LCDClear();
  int rand_top = randomInRange(1000,1100);
  LCDClear();
  LCDPuts("top: %i",rand_top);
  LCDPuts2("bot: %lu",getTime());
  _delay_ms(2500);
  }

}
