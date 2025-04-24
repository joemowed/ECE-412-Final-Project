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

int main(void) { /* Replace with your application code */
  LCDInit();
   LCDPuts("a string3, %i",3);
   LCDPuts2("a string3, %i",365);

  while (1) {
  int rand_top = randomInRange(1000,1100);
  int rand_bot = randomInRange(1000,1100);
  }
}
