/*
 * final project.c
 *
 * Created: 4/24/2025 10:46:21 AM
 * Author : AustJ
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "LCD.h"
#include "delay.h"
#include "random.h"
#include "time.h"
#include "portapi.h"

void StartBuzzer(void) {
  TCNT1 = 0x00;      // Start the timer count from 0;
  TCCR1A = 0x82;     // These configuration bits are key, research them...
	 TCCR1B = 0x1A;}
void StopBuzzer(void) {
  TCNT1 = 0x00;     
  TCCR1A = 0x00;     
	 TCCR1B = 0x00;
portWritePin(&PORTB,1,0); }

// Variables for ISR
volatile unsigned long reaction_time = 0;
volatile uint8_t winner = 0;

void pin_change_interrupt_init() {
  PCICR |= (1 << PCIE0);    // Enable PCINT[7:0]
  PCMSK0 |= (1 << PCINT4);  // Enable PCINT4 (PB4)
  PCMSK0 |= (1 << PCINT5);  // Enable PCINT5 (PB5)
  sei();                    // Enable global interrupts
}

// Pin Change Interrupt for PCINT0..7 (Port B)
ISR(PCINT0_vect) {
	const uint8_t read_PINB = PINB;
  if (!(read_PINB & (1 << PINB4))) {
    winner = 1;
  } else if (!(read_PINB & (1 << PINB5))) {
    winner = 2;
  }
  reaction_time = stopDrawTimer();
_delay_ms(50); // debounce the button
}

int main(void) {
  DDRB &= ~((1 << DDRB4) | (1 << DDRB5));  // PB4 and PB5 as input with pullup
  PORTB |= (1 << PORTB4) | (1 << PORTB5);  // PB4 and PB5 as input with pullup
  DDRB |= 0x02;      // set PB1 as output to drive transistor driver.
  ICR1 = 1075;       // Make TOP value so Frequency ~120HZ, see CLK pre-scaler
  TCNT1 = 0x00;      // Start the timer count from 0;
  TCCR1A = 0x82;     // These configuration bits are key, research them...
  TCCR1B = 0x18;     // ditto!
  OCR1A = ICR1 / 2;  // 50% duty cycle
  pin_change_interrupt_init();
  _delay_ms(2000);//wait, in case PC tries to talk over COM port
  LCDInit();
  timeStart();

  while (1) {
  LCDClear();
    LCDPuts("Push Any Button");
    LCDPuts2("to Start Game");
   winner = 0;
    while (winner == 0) {
    }
    winner = 0;

    LCDClear();
    LCDPuts("Get Ready....");
    int Rand = randomInRange(3000, 8000);
    unsigned long StartTime = getTime();
    while ((getTime() - StartTime) < Rand) {
    }

    LCDClear();
    LCDPuts("GO!!");
    LCDPuts2("GO!!");
    StartBuzzer();
    startDrawTimer();
    winner = 0;
    while (winner == 0) {
    }

   StopBuzzer();
   LCDClear();
    if (winner == 2) {
      LCDPuts("winner ==>");
    } else {
      LCDPuts("<== winner");
    }
    LCDPuts2("Time : %d", reaction_time);
    _delay_ms(5000);

  }
}
