/**
 * MARK HALL
 * ECEN 361
 * Lab 3: Arduino Timer
 * 
 * In this lab, we are required to create a timed interrupt:  As in, at regular intervals,
 * we run an interrupt.
 */

 // Libraries
#include <avr/io.h> 
#include <avr/interrupt.h> 


// They seem to do this in all the examples.  I suppose it beats remembering which pin you set
// everything to.  In these easy examples, you aren't going to forget, but I suppose that will change.
// I suppose I should concede and follow best practice.
#define LEDPIN_1 13
#define LEDPIN_2 12

// We have 2 LED's popping off at different times. We interrupt once, so we count it out.
volatile unsigned int countdown = 0;

void setup() {
  // setup the LED
  pinMode(LEDPIN_1, OUTPUT); 
  pinMode(LEDPIN_2, OUTPUT); 
  
  /**
   * Now we set up the timer.  A few interesting things to note here:  First, we turn off
   * global interrupts while we are setting this up.  I am not sure why we need to, because
   * we haven't turned them on yet, but I assume it is best practice, so I will keep it in
   * my code.
   * 
   * Next, you set TCCR1A and B to 0.  What are they?  They are timer control registers.
   * 
   * Then, we are enabling the overflow interrupt. Overflow as in... this is an unsigned int,
   * and when it hits max value, it will return to 0.  
   * 
   * Next, we set the timer, and finally reenable global interrupts.
   */

   TCCR1A = 0; 
   TCCR1B = 0; 

  // This is the countdown of the timer.  How long until overflow?
  // I have it set to 1/2 a second right now.  However, that could change by adjusting
  // the number, which is an unsigned int, so 2^16, I believe.
  OCR1A = 7812; 

  TCCR1B |= (1 << WGM12); 
  TCCR1B |= (1 << CS10); 
  TCCR1B |= (1 << CS12); 
  TIMSK1 |= (1 << OCIE1A); 
  sei(); 

}

void loop() {
  // Do nothing for the purpose of this lab
}

ISR(TIMER1_COMPA_vect) 
{ 
  countdown++;

  if (countdown % 4 == 0)
  digitalWrite(LEDPIN_1, !digitalRead(LEDPIN_1)); 
  if (countdown % 1 == 0)
  digitalWrite(LEDPIN_2, !digitalRead(LEDPIN_2));
}
