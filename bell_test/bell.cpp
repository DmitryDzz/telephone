#include "bell.h"

#include <Arduino.h>


#define portOfPin(P)\
  (((P) >= 0&&(P) < 8) ? &PORTD : (((P) > 7&&(P) < 14) ? &PORTB : &PORTC))
#define pinIndex(P) ((uint8_t) (P > 13 ? P - 14 : P&7))
#define pinMask(P) ((uint8_t) (1 << pinIndex(P)))
#define digitalLow(P) *(portOfPin(P)) &= ~pinMask(P)
#define digitalHigh(P) *(portOfPin(P)) |= pinMask(P)


// Base ticks (100 kHz ticks):
const uint8_t PULSE_COUNTER = 160;
// 4 kHz signal:
const long HIGH_FREQ_UP_TICKS = 8;
const long HIGH_FREQ_DOWN_TICKS = 25;
// 25 Hz signal:
const long LOW_FREQ_UP_TICKS = 2000;
const long LOW_FREQ_DOWN_TICKS = 4000;

bool enabled;

// Variants to save registries:
uint8_t tccr2a;
uint8_t tccr2b;
uint8_t tccn2;
uint8_t ocr2a;
uint8_t timsk2;

int highFreqPin;
long highFreqTicks;

int lowFreqPin;
long lowFreqTicks;


void Bell::initialize(const int highFrequencyPin, const int lowFrequencyPin) {
  enabled = false;
  
  highFreqPin = highFrequencyPin;
  lowFreqPin = lowFrequencyPin;
  pinMode(highFreqPin, OUTPUT);
  pinMode(lowFreqPin, OUTPUT);
}

/*
 * 16MHz is Arduino Uno's frequency.
 * Starting Arduino Uno's timer2.
 * No prescale, 100 kHz:
 */
void Bell::startSound() {
  if (enabled) {
    return;
  }

  cli();//stop interrupts

  Bell::saveRegistries();

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = PULSE_COUNTER;
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS20 bit for no prescaler
  TCCR2B |= (1 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  highFreqTicks = 0;
  lowFreqTicks = 0;
  digitalLow(highFreqPin);
  digitalLow(lowFreqPin);

  sei();//allow interrupts

  enabled = true;
}

void Bell::stopSound() {
  if (!enabled) {
    return;
  }

  cli();
  Bell::restoreRegistries();
  sei();

  digitalLow(highFreqPin);
  digitalLow(lowFreqPin);

  enabled = false;
}

void Bell::saveRegistries() {
  tccr2a = TCCR2A;
  tccr2a = TCCR2B;
  tccn2 = TCNT2;
  ocr2a = OCR2A;
  timsk2 = TIMSK2;
}

void Bell::restoreRegistries() {
  TCCR2A = tccr2a;
  TCCR2B = tccr2a;
  TCNT2 = tccn2;
  OCR2A = ocr2a;
  TIMSK2 = timsk2;
}

ISR(TIMER2_COMPA_vect) {
  highFreqTicks++;
  if (highFreqTicks == HIGH_FREQ_UP_TICKS) {
    digitalHigh(highFreqPin);
  } else if (highFreqTicks == HIGH_FREQ_DOWN_TICKS) {
    digitalLow(highFreqPin);
    highFreqTicks = 0;
  }
  
  lowFreqTicks++;
  if (lowFreqTicks == LOW_FREQ_UP_TICKS) {
    digitalHigh(lowFreqPin);
  } else if (lowFreqTicks == LOW_FREQ_DOWN_TICKS) {
    digitalLow(lowFreqPin);
    lowFreqTicks = 0;
  }
}
