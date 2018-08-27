#include "timer_interrupt.h"

void timer_routine();
enum rasce_state

void setup() {
  timer_init(&timer_routine); // init timer interrupt for time measurment
}

void loop() {
  // put your main code here, to run repeatedly:

}

void timer_routine()
{
  
}

