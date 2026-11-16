#pragma once
// All tunable hardware and network settings live here.
// Edit values here; do not hard-code pins or credentials in the .cpp files.

#define SERIAL_BAUD 115200

// ---- Sonar: RCWL-1601 (trigger/echo mode, powered from the 3.3 V rail) ----
#define SONAR_TRIG_PIN 26              // ESP32 output: 10 us HIGH pulse starts a ping
#define SONAR_ECHO_PIN 27              // ESP32 input: HIGH for the round-trip time
#define SONAR_ECHO_TIMEOUT_US 30000UL  // ~5 m round trip; no echo before this = "no reading"
#define SONAR_MAX_CM 400               // readings beyond this are reported as "no reading"

// ---- Servo (PWM) ----
#define SERVO_PIN 18                   // PWM signal wire of the servo
#define SERVO_MIN_US 500               // pulse width at 0 degrees (check your servo datasheet)
#define SERVO_MAX_US 2400              // pulse width at 180 degrees

// ---- Sweep ----
#define SWEEP_MIN_DEG 0
#define SWEEP_MAX_DEG 180
#define SWEEP_STEP_DEG 2               // degrees per step
#define SWEEP_DWELL_MS 30              // time the servo is given to settle before each reading

// ---- Wi-Fi ----
// 1 = the ESP32 creates its own network (default; connect laptop/phone to it, map at http://192.168.4.1)
// 0 = the ESP32 joins an existing network named WIFI_STA_SSID
#define WIFI_USE_AP 1

#define WIFI_AP_SSID "sonar-mapper"    // network name when WIFI_USE_AP is 1
#define WIFI_AP_PASS "sonarmap123"     // at least 8 characters, or "" for an open network

#define WIFI_STA_SSID "your-network"   // home/lab network name when WIFI_USE_AP is 0
#define WIFI_STA_PASS "your-password"  // password for WIFI_STA_SSID
#define WIFI_STA_TIMEOUT_MS 15000      // how long to wait for the join before giving up
