#pragma once

// Configures the servo and moves it to SWEEP_MIN_DEG. Call once from setup().
void sweepInit();

// True once the servo has dwelled at sweepAngle() long enough to take a reading.
bool sweepSettled();

// Moves the servo to the next step (0 -> 180 -> 0, step and dwell from config.h).
// Call after taking the reading for the current angle.
void sweepAdvance();

// Angle, in degrees, the servo is currently pointing at.
int sweepAngle();
