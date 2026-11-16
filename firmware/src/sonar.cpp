#include <Arduino.h>
#include "config.h"
#include "sonar.h"

void sonarInit() {
  pinMode(SONAR_TRIG_PIN, OUTPUT);
  digitalWrite(SONAR_TRIG_PIN, LOW);
  pinMode(SONAR_ECHO_PIN, INPUT);
}

int sonarReadCm() {
  // 10 us trigger pulse after a short low settle
  digitalWrite(SONAR_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SONAR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONAR_TRIG_PIN, LOW);

  // Echo pulse width is the round trip time. pulseIn returns 0 on timeout.
  unsigned long us = pulseIn(SONAR_ECHO_PIN, HIGH, SONAR_ECHO_TIMEOUT_US);
  if (us == 0) {
    return SONAR_NO_READING;
  }

  // Sound covers ~0.0343 cm/us; the pulse is the round trip, so halve it.
  int cm = (int)(us * 0.0343f / 2.0f + 0.5f);
  if (cm > SONAR_MAX_CM) {
    return SONAR_NO_READING;
  }
  return cm;
  // TODO(sonar): validate the speed-of-sound constant and offset against the tape-measure test in tests/README.md
}
