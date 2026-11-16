#pragma once
#include <Arduino.h>

// Starts Wi-Fi (AP or station per WIFI_USE_AP), mounts LittleFS, and starts
// the web server with the /ws WebSocket. Call once from setup().
void netInit();

// Broadcasts one reading to all WebSocket clients as:
//   {"angle":<deg>,"dist_cm":<cm or -1>,"t":<ms>}
void netPublish(int angleDeg, int distCm, unsigned long tMs);
