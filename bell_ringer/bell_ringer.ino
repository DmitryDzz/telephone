#include "bell.h"

const int HIGH_FREQUENCY_PIN = 12;
const int LOW_FREQUENCY_A_PIN = 10;
const int LOW_FREQUENCY_B_PIN = 11;
const long DURATION = 1L * 60L * 1000L;
bool started;
long startMillis;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\r");

  Bell::initialize(HIGH_FREQUENCY_PIN, LOW_FREQUENCY_A_PIN, LOW_FREQUENCY_B_PIN);

  Bell::start();
  started = true;
  startMillis = millis();
  Serial.print("Started...");
}

void loop() {
//  delay(1000);
//  Serial.println("Hi!");
  if (started && (millis() - startMillis >= DURATION)) {
    Bell::stop();
    started = false;
    Serial.println("stopped");
  }
}
