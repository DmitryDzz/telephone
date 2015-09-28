#include "button.h"

using namespace telephone;

const int HANDSET_PIN = A0;
const int ROTATE_PIN = A1;
const int COUNTER_PIN = A2;
const int LED_PIN = 13;

Button buttonHandset(0, 1023);
Button buttonRotate(0, 1023);
Button buttonCounter(0, 1023);

long currentMillis;

void buttonLedHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    digitalWrite(LED_PIN, HIGH);
  } else if (buttonState == RELEASED) {
    digitalWrite(LED_PIN, LOW);
  }
}

void buttonSerialHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    Serial.println("PRESSED");
  } else if (buttonState == RELEASED) {
    Serial.println("RELEASED");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(HANDSET_PIN, INPUT);
  pinMode(ROTATE_PIN, INPUT);
  pinMode(COUNTER_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  delay(100);
  buttonHandset.setHandler(buttonLedHandler);
  buttonRotate.setHandler(buttonLedHandler);
  buttonCounter.setHandler(buttonSerialHandler);
}

void loop() {
  currentMillis = millis();
  buttonHandset.refresh(analogRead(HANDSET_PIN), currentMillis);
  buttonRotate.refresh(analogRead(ROTATE_PIN), currentMillis);
  buttonCounter.refresh(analogRead(COUNTER_PIN), currentMillis);
}
