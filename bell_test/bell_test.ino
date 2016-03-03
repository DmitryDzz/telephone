#include "bell.h"

const int HIGH_FREQUENCY_PIN = 8;
const int LOW_FREQUENCY_PIN = 9;
const long DURATION = 1L * 60L * 1000L;
bool started;
long startMillis;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\r");

  Bell::initialize(HIGH_FREQUENCY_PIN, LOW_FREQUENCY_PIN);

  Bell::startSound();
  started = true;
  startMillis = millis();
  Serial.print("Started...");
}

void loop() {
  if (started && (millis() - startMillis >= DURATION)) {
//    Bell::stopSound();
    started = false;
    Serial.println("stopped");
  }
}
