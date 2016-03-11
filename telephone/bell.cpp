#include "bell.h"

#include <Arduino.h>


#define portOfPin(P)\
  (((P) >= 0&&(P) < 8) ? &PORTD : (((P) > 7&&(P) < 14) ? &PORTB : &PORTC))
#define pinIndex(P) ((uint8_t) (P > 13 ? P - 14 : P&7))
#define pinMask(P) ((uint8_t) (1 << pinIndex(P)))
#define digitalLow(P) *(portOfPin(P)) &= ~pinMask(P)
#define digitalHigh(P) *(portOfPin(P)) |= pinMask(P)


// Base ticks (1 kHz ticks = 249):
const uint8_t PULSE_COUNTER = 249;
// 0.25 Hz 50% beep signal (4 seconds beep, 4 seconds silince):
const long BEEP_FREQ_PERIOD_TICKS = 8000L;
const long BEEP_FREQ_UP_TICKS = 0L;
const long BEEP_FREQ_DOWN_TICKS = 4000L;
// 25 Hz 50% signal (LOW_FREQ_UP_TICKS=2000, LOW_FREQ_DOWN_TICKS=4000):
const long LOW_FREQ_PERIOD_TICKS = 40L;
const long LOW_FREQ_ZERO_DELAY_TICKS = 3L;
const long LOW_FREQ_UP_A_TICKS = LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_DOWN_A_TICKS = LOW_FREQ_PERIOD_TICKS / 2L - LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_UP_B_TICKS = LOW_FREQ_PERIOD_TICKS / 2L + LOW_FREQ_ZERO_DELAY_TICKS;
const long LOW_FREQ_DOWN_B_TICKS = LOW_FREQ_PERIOD_TICKS - LOW_FREQ_ZERO_DELAY_TICKS;

bool enabled;

// Variants to save registries:
uint8_t tccr2a;
uint8_t tccr2b;
uint8_t tccn2;
uint8_t ocr2a;
uint8_t timsk2;

long beepFreqTicks;
boolean beeping;

int highFreqActivityPin;

int lowFreqPinA;
int lowFreqPinB;
long lowFreqTicks;


void Bell::initialize(const int highFrequencyActivityPin,
                      const int lowFrequencyPinA,
                      const int lowFrequencyPinB) {
  enabled = false;
  beeping = false;
  
  highFreqActivityPin = highFrequencyActivityPin;
  lowFreqPinA = lowFrequencyPinA;
  lowFreqPinB = lowFrequencyPinB;
  pinMode(highFreqActivityPin, OUTPUT);
  pinMode(lowFreqPinA, OUTPUT);
  pinMode(lowFreqPinB, OUTPUT);
}

/*
 * 16MHz is Arduino Uno's frequency.
 * Starting Arduino Uno's timer2.
 * No prescale, 100 kHz:
 */
void Bell::start() {
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
  // Set CS22 bit for clk/64 prescaler
  TCCR2B |= (1 << CS22);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  beepFreqTicks = 0;
  beeping = false;
  lowFreqTicks = 0;
  digitalLow(highFreqActivityPin);
  digitalLow(lowFreqPinA);
  digitalLow(lowFreqPinB);

  sei();//allow interrupts

  enabled = true;
}

void Bell::stop() {
  if (!enabled) {
    return;
  }

  cli();
  Bell::restoreRegistries();
  sei();

  beeping = false;
  enabled = false;
  digitalLow(highFreqActivityPin);
  digitalLow(lowFreqPinA);
  digitalLow(lowFreqPinB);
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
  if (beepFreqTicks == BEEP_FREQ_UP_TICKS) {
    beeping = true;
    lowFreqTicks = 0;
    digitalHigh(highFreqActivityPin);
  } else if (beepFreqTicks == BEEP_FREQ_DOWN_TICKS) {
    beeping = false;
    lowFreqTicks = 0;
    digitalLow(highFreqActivityPin);
    digitalLow(lowFreqPinA);
    digitalLow(lowFreqPinB);
  }
  if (++beepFreqTicks >= BEEP_FREQ_PERIOD_TICKS) {
    beepFreqTicks = 0;
  }
  if (!beeping) return;
  
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
  if (++lowFreqTicks >= LOW_FREQ_PERIOD_TICKS) {
    lowFreqTicks = 0;
  }
}
