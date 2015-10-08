#include "button.h"
#include <GSM.h>

using namespace telephone;

const int HANDSET_PIN = A0;
const int ROTATE_PIN = A1;
const int COUNTER_PIN = A2;
const int LED_PIN = 13;
const int AUTO_START_GSM_PIN = 8;

Button buttonHandset(0, 1023);
Button buttonRotate(0, 1023);
Button buttonCounter(0, 1023);

boolean isGettingPhoneNumber;
boolean isGettingDigit;
boolean isTalking;

int digitCounter;
int digitValue;
String phoneNumber;

const int DIGITS_IN_PHONE_NUMBER = 11;
char phoneNumberChars[DIGITS_IN_PHONE_NUMBER + 1];

//---- gsm begin
#define SIM_PIN_NUMBER ""
GSM gsmAccess;
GSMVoiceCall gsmVoiceCall;
//---- gsm end

void pickedUpHandler(telephone::ButtonState buttonState) {
  isGettingPhoneNumber = buttonState == PRESSED;
  if (isGettingPhoneNumber) {
    phoneNumber = "";
    digitCounter = 0;
  } else {
    isGettingDigit = false;
    if (isTalking) {
      gsmVoiceCall.hangCall();
      isTalking = false;
    }
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
  phoneNumber.toCharArray(phoneNumberChars, DIGITS_IN_PHONE_NUMBER + 1);
  if (gsmVoiceCall.voiceCall(phoneNumberChars)) {
    isTalking = true;
    while (isTalking && (gsmVoiceCall.getvoiceCallStatus() == TALKING));
    // ПРОВЕРИТЬ: когда трубку кладут "тут" и когда кладут "там".
    // ПРОВЕРИТЬ: когда занято.
  }
}

void updatePhoneNumberForRussia() {
  if (phoneNumber[0] == '8') {
    phoneNumber[0] = '7';
    phoneNumber = "+" + phoneNumber;
  }
}

void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println("Started");

  pinMode(HANDSET_PIN, INPUT);
  pinMode(ROTATE_PIN, INPUT);
  pinMode(COUNTER_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(AUTO_START_GSM_PIN, OUTPUT);
  delay(100);
  digitalWrite(AUTO_START_GSM_PIN, HIGH);

  buttonHandset.setHandler(pickedUpHandler);
  buttonRotate.setHandler(rotatingHandler);
  buttonCounter.setHandler(rotaryCounterHandler);
  
  while (true) {
    if (gsmAccess.begin(SIM_PIN_NUMBER) == GSM_READY) {
      break;
    }
    delay(5000);
  }
  
  Serial.println("Connected to network");
}

void loop() {
  const long currentMillis = millis();
  buttonHandset.refresh(analogRead(HANDSET_PIN), currentMillis);
  buttonRotate.refresh(analogRead(ROTATE_PIN), currentMillis);
  buttonCounter.refresh(analogRead(COUNTER_PIN), currentMillis);
}
