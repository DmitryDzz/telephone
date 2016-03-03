#include "bell.h"

#include <Arduino.h>


#define portOfPin(P)\
  (((P) >= 0&&(P) < 8) ? &PORTD : (((P) > 7&&(P) < 14) ? &PORTB : &PORTC))
#define pinIndex(P) ((uint8_t) (P > 13 ? P - 14 : P&7))
#define pinMask(P) ((uint8_t) (1 << pinIndex(P)))
#define digitalLow(P) *(portOfPin(P)) &= ~pinMask(P)
#define digitalHigh(P) *(portOfPin(P)) |= pinMask(P)


// Base ticks (100 kHz ticks = 160):
const uint8_t PULSE_COUNTER = 160;
// 4 kHz signal (HIGH_FREQ_UP_TICKS=5, HIGH_FREQ_DOWN_TICKS=25):
// 4.2 kHz, 89.6% duty cycle (HIGH_FREQ_UP_TICKS=3, HIGH_FREQ_DOWN_TICKS=21):
const long HIGH_FREQ_PERIOD_TICKS = 26;
const long HIGH_FREQ_UP_TICKS = 2;
const long HIGH_FREQ_DOWN_TICKS = HIGH_FREQ_PERIOD_TICKS - HIGH_FREQ_UP_TICKS;
// 25 Hz 50% signal (LOW_FREQ_UP_TICKS=2000, LOW_FREQ_DOWN_TICKS=4000):
const long LOW_FREQ_PERIOD_TICKS = 4000;
const long LOW_FREQ_ZERO_DELAY_TICKS = 100;
const long LOW_FREQ_UP_A_TICKS = LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_DOWN_A_TICKS = LOW_FREQ_PERIOD_TICKS / 2 - LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_UP_B_TICKS = LOW_FREQ_PERIOD_TICKS / 2 + LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_DOWN_B_TICKS = LOW_FREQ_PERIOD_TICKS - LOW_FREQ_ZERO_DELAY_TICKS;

bool enabled;

// Variants to save registries:
uint8_t tccr2a;
uint8_t tccr2b;
uint8_t tccn2;
uint8_t ocr2a;
uint8_t timsk2;

int highFreqPin;
long highFreqTicks;

int lowFreqPinA;
int lowFreqPinB;
long lowFreqTicks;


void Bell::initialize(const int highFrequencyPin, const int lowFrequencyPinA, const int lowFrequencyPinB) {
  enabled = false;
  
  highFreqPin = highFrequencyPin;
  lowFreqPinA = lowFrequencyPinA;
  lowFreqPinB = lowFrequencyPinB;
  pinMode(highFreqPin, OUTPUT);
  pinMode(lowFreqPinA, OUTPUT);
  pinMode(lowFreqPinB, OUTPUT);
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
  digitalLow(lowFreqPinA);
  digitalLow(lowFreqPinB);

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
  digitalLow(lowFreqPinA);
  digitalLow(lowFreqPinB);

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
  if (lowFreqTicks == LOW_FREQ_UP_A_TICKS) {
    digitalHigh(lowFreqPinA);
  } else if (lowFreqTicks == LOW_FREQ_DOWN_A_TICKS) {
    digitalLow(lowFreqPinA);
  }
  if (lowFreqTicks == LOW_FREQ_UP_B_TICKS) {
    digitalHigh(lowFreqPinB);
  } else if (lowFreqTicks == LOW_FREQ_DOWN_B_TICKS) {
    digitalLow(lowFreqPinB);
  }
  if (lowFreqTicks == LOW_FREQ_PERIOD_TICKS) {
    lowFreqTicks = 0;
  }
}
