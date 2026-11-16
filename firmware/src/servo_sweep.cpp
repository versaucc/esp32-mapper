#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"
#include "servo_sweep.h"

static Servo servo;
static int angle = SWEEP_MIN_DEG;
static int direction = +1;
static unsigned long lastMoveMs = 0;

void sweepInit() {
  servo.setPeriodHertz(50);  // standard hobby servo: 50 Hz
  servo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  angle = SWEEP_MIN_DEG;
  servo.write(angle);
  lastMoveMs = millis();
  // TODO(servo): confirm SERVO_MIN_US / SERVO_MAX_US give a full 0-180 sweep with the actual servo
}

bool sweepSettled() {
  return millis() - lastMoveMs >= SWEEP_DWELL_MS;
}

void sweepAdvance() {
  angle += direction * SWEEP_STEP_DEG;
  if (angle >= SWEEP_MAX_DEG) {
    angle = SWEEP_MAX_DEG;
    direction = -1;
  } else if (angle <= SWEEP_MIN_DEG) {
    angle = SWEEP_MIN_DEG;
    direction = +1;
  }
  servo.write(angle);
  lastMoveMs = millis();
}

int sweepAngle() {
  return angle;
}
