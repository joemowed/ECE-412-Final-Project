/*
 * final project.c
 *
 * Created: 4/24/2025 10:46:21 AM
 * Author : AustJ
 */

#include <avr/io.h>

#include "LCD.h"
#include "delay.h"
#include "time.h"
#include "random.h"
#include <util/delay.h>
#define F_CPU 16000000UL
#include <stdlib.h>
#include <avr/interrupt.h>

void StartBuzzer(void)
{
	TCCR1B = 0x1A;
}
void StopBuzzer(void)
{
	TCCR1B = 0x18;
}

// Variables for ISR
volatile uint16_t reaction_time = 0;
volatile uint8_t winner = 0;

void pin_change_interrupt_init() {
	PCICR |= (1 << PCIE0);       // Enable PCINT[7:0]
	PCMSK0 |= (1 << PCINT4);     // Enable PCINT0 (PB0)
	PCMSK0 |= (1 << PCINT5);     // Enable PCINT1 (PB1)
	sei();                       // Enable global interrupts
}

// Pin Change Interrupt for PCINT0..7 (Port B)
ISR(PCINT0_vect) {
	if (!(PINB & (1 << PB4))) {
		winner = 1;
	}
	else if (!(PINB & (1 << PB5))) {
		winner = 2;
	}

}

int main(void) { 
  double final;
  DDRB &= ~((1 << PORTB4) | (1 << PORTB5));
  PORTB |= (1 << PORTB4) | (1 << PORTB5);
  DDRB |= 0x02;		//set PB1 as output to drive transistor driver.
  ICR1 = 2075;		//Make TOP value so Frequency ~120HZ, see CLK pre-scaler
  TCNT1 = 0x00;			//Start the timer count from 0;
  TCCR1A = 0x82;		//These configuration bits are key, research them...
  TCCR1B = 0x1A;		//ditto!
  TIMSK1 |= 0x02;		//Enable Timer 1 to generate interrupts.
  LCDInit();
  timeStart();

  while (1) {
	LCDPuts("Push Any Button");
	while (winner == 0)
	{
	}
	winner = 0;
	LCDPuts("Wait 4 Buzzer");
	int Rand = randomInRange(3000,8000);
	unsigned long StartTime = getTime();
	while((getTime()-StartTime) < Rand)
	{
	}
	StartBuzzer();
	startDrawTimer();
	while(winner == 0)
	{
	}
	StopBuzzer();
	final = stopDrawTimer();
	if (winner == 1)
	{
		LCDPuts("winner =>");
	}
	else
	{
		LCDPuts("<= winner");
	}
	LCDPuts2("Time : %d", &final);
  }

}
