#include "button.h"
#include "bell.h"
#include <GSM.h>

using namespace telephone;

const int HANDSET_PIN = A0;
const int ROTATE_PIN = A1;
const int COUNTER_PIN = A2;
const int ASTERISK_PIN = A3;
const int NUMBER_SIGN_PIN = A4;
const int PLUS_PIN = A5;
const int NO_NETWORK_LED_PIN = 13;
const int AUTO_START_GSM_PIN = 8;
const int BELL_HIGH_FREQUENCY_ACTIVITY_PIN = 2;
const int BELL_LOW_FREQUENCY_A_PIN = 4;
const int BELL_LOW_FREQUENCY_B_PIN = 7;


Button buttonHandset(0, 1023);
Button buttonRotate(0, 1023);
Button buttonCounter(0, 1023);
Button buttonAsterisk(0, 1023);
Button buttonNumberSign(0, 1023);
Button buttonPlus(0, 1023);
Button incomingCall(false, true);

boolean isHeadsetUp;
boolean isGettingDigit;
boolean isTalking;
boolean hasIncomingCall;

int digitValue;
String phoneNumber;

char phoneNumberChars[20]; // 20 - "big" number. Greater than any pnone number.

#define SIM_PIN_NUMBER ""
GSM gsmAccess;
GSMVoiceCall gsmVoiceCall;

void pickedUpHandler(telephone::ButtonState buttonState) {
  isHeadsetUp = buttonState == PRESSED;
  if (isHeadsetUp) {
    if (!isTalking) {
      Serial.println("Headset up");
      phoneNumber = "";
      if (hasIncomingCall) {
        Bell::stop();
        if (gsmVoiceCall.answerCall()) {
          isTalking = true;
          while (isTalking && (gsmVoiceCall.getvoiceCallStatus() == TALKING)) {
            refreshButtons(millis());
          }
          isTalking = false;
          gsmVoiceCall.hangCall();
        }
      }
    }
  } else {
    Serial.println("Headset down");
    isGettingDigit = false;
    if (isTalking) {
      isTalking = false;
    }
  }
}

void rotatingHandler(telephone::ButtonState buttonState) {
  if (isHeadsetUp) {
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

void asteriskHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    Serial.println("Asterisk pressed");
    addSymbolToPhoneNumber("*");
  }
}

void numberSignHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    Serial.println("Number sign pressed");
    addSymbolToPhoneNumber("#");
  }
}

void plusHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    Serial.println("Plus pressed");
    addSymbolToPhoneNumber("+");
  }
}

void addSymbolToPhoneNumber(String symbol) {
  if (isHeadsetUp && (!isGettingDigit)) {
    phoneNumber += symbol;
    if (isTalking) {
        sendDigit();
    }
  }
}

//todo ButtonState???
void incomingCallHandler(telephone::ButtonState buttonState) {
  if (buttonState == PRESSED) {
    if (!isHeadsetUp) {
      hasIncomingCall = true;
      Bell::start();
      gsmVoiceCall.retrieveCallingNumber(phoneNumberChars, 20);
      Serial.print("Incoming call: ");
      Serial.println(phoneNumberChars);
    }
  } else {
    if (hasIncomingCall) {
      Bell::stop();
    }
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
  if (isTalking) {
    return;
  }
  Serial.println("Making call: " + phoneNumber);
  updatePhoneNumberForRussia();
  phoneNumber.toCharArray(phoneNumberChars, phoneNumber.length() + 1);
  phoneNumber = "";
  if (gsmVoiceCall.voiceCall(phoneNumberChars)) {
    isTalking = true;
    while (isTalking && (gsmVoiceCall.getvoiceCallStatus() == TALKING)) {
      refreshButtons(millis());
    }
    isTalking = false;
    gsmVoiceCall.hangCall();
    // ПРОВЕРИТЬ: когда занято.
  }
}

void sendDigit() {
  if (gsmVoiceCall.getvoiceCallStatus() != TALKING) {
    return;
  }
  if (phoneNumber == "") {
    return;
  }

  String atCommand = "AT+VTS=\"" + phoneNumber + "\"";
  Serial.println("Sending digit: " + phoneNumber + " at command: " + atCommand);

  theGSM3ShieldV1ModemCore.println(atCommand);
  
  phoneNumber = "";
}

void updatePhoneNumberForRussia() {
  if (phoneNumber[0] == '8') {
    phoneNumber[0] = '7';
    phoneNumber = "+" + phoneNumber;
  }
}

void refreshButtons(long currentMillis) {
  buttonHandset.refresh(analogRead(HANDSET_PIN), currentMillis);
  buttonRotate.refresh(analogRead(ROTATE_PIN), currentMillis);
  buttonCounter.refresh(analogRead(COUNTER_PIN), currentMillis);
  buttonAsterisk.refresh(analogRead(ASTERISK_PIN), currentMillis);
  buttonNumberSign.refresh(analogRead(NUMBER_SIGN_PIN), currentMillis);
  buttonPlus.refresh(analogRead(PLUS_PIN), currentMillis);
  incomingCall.refresh(gsmVoiceCall.getvoiceCallStatus() == RECEIVINGCALL, currentMillis);
}

void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println("Started");

  Bell::initialize(BELL_HIGH_FREQUENCY_ACTIVITY_PIN, BELL_LOW_FREQUENCY_A_PIN, BELL_LOW_FREQUENCY_B_PIN);
  
  pinMode(HANDSET_PIN, INPUT);
  pinMode(ROTATE_PIN, INPUT);
  pinMode(COUNTER_PIN, INPUT);
  pinMode(ASTERISK_PIN, INPUT);
  pinMode(NUMBER_SIGN_PIN, INPUT);
  pinMode(PLUS_PIN, INPUT);
  pinMode(NO_NETWORK_LED_PIN, OUTPUT);
  pinMode(AUTO_START_GSM_PIN, OUTPUT);
  delay(100);
  digitalWrite(NO_NETWORK_LED_PIN, HIGH);
  digitalWrite(AUTO_START_GSM_PIN, HIGH);

  buttonHandset.setHandler(pickedUpHandler);
  buttonRotate.setHandler(rotatingHandler);
  buttonCounter.setHandler(rotaryCounterHandler);
  buttonAsterisk.setHandler(asteriskHandler);
  buttonNumberSign.setHandler(numberSignHandler);
  buttonPlus.setHandler(plusHandler);
  incomingCall.setHandler(incomingCallHandler);
  
  Serial.print("Connecting..");
  while (true) {
    Serial.print(".");
    if (gsmAccess.begin(SIM_PIN_NUMBER) == GSM_READY) {
      Serial.println("done");
      break;
    }
    delay(1000);
  }
  digitalWrite(NO_NETWORK_LED_PIN, LOW);
}

void loop() {
  refreshButtons(millis());
}
