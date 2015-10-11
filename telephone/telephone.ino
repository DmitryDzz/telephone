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

int digitValue;
String phoneNumber;

char phoneNumberChars[20]; // 20 - "big" number. Greater than any pnone number.

//---- gsm begin
#define SIM_PIN_NUMBER ""
GSM gsmAccess;
GSMVoiceCall gsmVoiceCall;
//---- gsm end

void pickedUpHandler(telephone::ButtonState buttonState) {
  isGettingPhoneNumber = buttonState == PRESSED;
  if (isGettingPhoneNumber) {
    Serial.println("Headset up");
    phoneNumber = "";
  } else {
    Serial.println("Headset down");
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
      digitValue = 0;
    } else {
      if (digitValue > 0) {
        if (digitValue == 10) {
          digitValue = 0;
        }
        phoneNumber += String(digitValue);
        digitValue = 0;
      }
      if (isTalking) {
        sendDigit();
      } else if (isValidPhoneNumber()) {
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

// Adaptered for Russian Beeline operator.
// Modify this function for your country or operator.
boolean isValidPhoneNumber() {
  int phoneNumberSize = phoneNumber.length();
  if (phoneNumberSize == 4) { // Russian Beeline mobile operator short number: 06XX
    return (phoneNumber[0] == '0') && (phoneNumber[1] == '6');
  } else if (phoneNumberSize == 11) { // 8xxxxxxxxxx
    return phoneNumber[0] == '8';
  } else if (phoneNumberSize == 12) { // +7XXXxxxxxxx call to Russia
    return (phoneNumber[0] == '+') && (phoneNumber[1] == '7');
  } else {
    return false;
  }
}

void makeCall() {
  Serial.println("Making call: " + phoneNumber);
  updatePhoneNumberForRussia();
  phoneNumber.toCharArray(phoneNumberChars, phoneNumber.length() + 1);
  phoneNumber = "";
  if (gsmVoiceCall.voiceCall(phoneNumberChars)) {
    isTalking = true;
    while (isTalking && (gsmVoiceCall.getvoiceCallStatus() == TALKING)) {
      refreshButtons();
    }
    // ПРОВЕРИТЬ: когда трубку кладут "тут" и когда кладут "там".
    // ПРОВЕРИТЬ: когда занято.
  }
}

void sendDigit() {
  if (phoneNumber == "") {
    return;
  }
  Serial.println("Sending digit: " + phoneNumber);
  phoneNumber = "";
}

void updatePhoneNumberForRussia() {
  if (phoneNumber[0] == '8') {
    phoneNumber[0] = '7';
    phoneNumber = "+" + phoneNumber;
  }
}

void refreshButtons() {
  const long currentMillis = millis();
  buttonHandset.refresh(analogRead(HANDSET_PIN), currentMillis);
  buttonRotate.refresh(analogRead(ROTATE_PIN), currentMillis);
  buttonCounter.refresh(analogRead(COUNTER_PIN), currentMillis);
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
  
  Serial.print("Connecting..");
  while (true) {
    Serial.print(".");
    if (gsmAccess.begin(SIM_PIN_NUMBER) == GSM_READY) {
      Serial.println("done");
      break;
    }
    delay(1000);
  }
}

void loop() {
  refreshButtons();
}
