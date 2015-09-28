#include "button.h"

using namespace telephone;

const int HANDSET_PIN = A0;
const int ROTATE_PIN = A1;
const int COUNTER_PIN = A2;
const int LED_PIN = 13;

Button buttonHandset(0, 1023);
Button buttonRotate(0, 1023);
Button buttonCounter(0, 1023);

boolean isGettingPhoneNumber;
boolean isGettingDigit;

int digitCounter;
int digitValue;
String phoneNumber;

const int DIGITS_IN_PHONE_NUMBER = 11;

void pickedUpHandler(telephone::ButtonState buttonState) {
  isGettingPhoneNumber = buttonState == PRESSED;
  if (isGettingPhoneNumber) {
    phoneNumber = "";
    digitCounter = 0;
  } else {
    isGettingDigit = false;
  }
}

void rotatingHandler(telephone::ButtonState buttonState) {
  if (isGettingPhoneNumber) {
    isGettingDigit = buttonState == PRESSED;
    if (isGettingDigit) {
      digitCounter++;
      digitValue = 0;
    } else {
      if (digitValue == 10) {
        digitValue = 0;
      }
      phoneNumber += String(digitValue);
      if (digitCounter == DIGITS_IN_PHONE_NUMBER) {
        makeCall();
      }
    }
  }
}

void rotaryCounterHandler(telephone::ButtonState buttonState) {
  if (isGettingDigit && (buttonState == RELEASED)) {
    digitValue++;
  }
}

void makeCall() {
  updatePhoneNumberForRussia();
  Serial.println("Phone number: " + phoneNumber);
}

void updatePhoneNumberForRussia() {
  if (phoneNumber[0] == '8') {
    phoneNumber[0] = '7';
    phoneNumber = "+" + phoneNumber;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(HANDSET_PIN, INPUT);
  pinMode(ROTATE_PIN, INPUT);
  pinMode(COUNTER_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  delay(100);
  buttonHandset.setHandler(pickedUpHandler);
  buttonRotate.setHandler(rotatingHandler);
  buttonCounter.setHandler(rotaryCounterHandler);
}

void loop() {
  const long currentMillis = millis();
  buttonHandset.refresh(analogRead(HANDSET_PIN), currentMillis);
  buttonRotate.refresh(analogRead(ROTATE_PIN), currentMillis);
  buttonCounter.refresh(analogRead(COUNTER_PIN), currentMillis);
}
