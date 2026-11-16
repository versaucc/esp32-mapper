#include <Arduino.h>
#include "config.h"
#include "net.h"
#include "servo_sweep.h"
#include "sonar.h"

void setup() {
  Serial.begin(SERIAL_BAUD);
  sonarInit();
  sweepInit();
  netInit();
}

void loop() {
  // Each step: wait for the servo to settle, take one reading, send it, then move on.
  if (sweepSettled()) {
    int angle = sweepAngle();
    int distCm = sonarReadCm();
    netPublish(angle, distCm, millis());
    sweepAdvance();
  }
  // TODO(net): the web server and WebSocket run in the background; keep loop() free of blocking calls
}
