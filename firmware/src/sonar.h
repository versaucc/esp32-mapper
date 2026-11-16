#pragma once

#define SONAR_NO_READING -1

// Configures the trigger and echo pins. Call once from setup().
void sonarInit();

// Fires one ping and returns the distance to the nearest object in centimetres.
// Returns SONAR_NO_READING on timeout (no echo) or if the distance exceeds SONAR_MAX_CM.
int sonarReadCm();
