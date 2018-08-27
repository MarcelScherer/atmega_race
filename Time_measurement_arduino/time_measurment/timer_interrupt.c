#include <avr/interrupt.h>
#include <Arduino.h>
#include "timer_interrupt.h"

static void (*interrupt_routine)();

uint8_t actaul_timer[3]; // actaul timer array [minute], [secunde] and [0.1 secundes]

void timer_init( void (*interrupt_func)(void))
{
  noInterrupts();           // disable all interrupts

//set timer1 interrupt at 100Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 100hz increments
  OCR1A = 19999;// = (16*10^6) / (100*8) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 8 prescaler
  TCCR1B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  interrupt_routine = interrupt_func;

  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect)		//Interrupt-Routine wird alle 5ms aufgerufen
{
  cli();							//Interrupts deaktivieren
  interrupt_routine();
  sei();							//Interrupts aktivieren
}





