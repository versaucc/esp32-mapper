# Bench test plan

Run these on the assembled bench (ESP32, RCWL-1601, servo) before the demo.
Save raw data as CSV in `tests/logs/` with the date and test name, for example
`2026-11-20_distance-accuracy.csv`. Each test has an owner from the subsystem list in
[CONTRIBUTING.md](../CONTRIBUTING.md).

## 1. Distance accuracy (owner: sonar)

**Goal:** the reported `dist_cm` matches a tape measure.

**Setup:** fix a flat board perpendicular to the sensor, at 20, 50, 100, 200 and 300 cm.
Hold the servo at 90° (or set the sweep so it dwells on one angle).

**Procedure:** take 20 readings at each distance and record the median and spread.

**Pass:** median within ±3 cm for 20-300 cm. No-echo rate under 5% at 300 cm.

## 2. Angle accuracy (owner: servo)

**Goal:** the angle reported matches where the servo is actually pointing.

**Setup:** mark a protractor or paper scale under the servo horn. Attach a 10 cm
pointer to the horn.

**Procedure:** step the servo to 0, 45, 90, 135 and 180°. Check the pointer against the
mark and check the `angle` value in the serial monitor.

**Pass:** pointer within ±3° of the marked angle at each position. The reported
`angle` is always the commanded value (the servo has no feedback).

## 3. Update rate (owner: net)

**Goal:** the browser receives readings at the rate the firmware sends them.

**Setup:** connect a laptop to the ESP32 access point. Open the page and the browser
dev tools console.

**Procedure:** log `ws.onmessage` timestamps for 60 s. Count messages per second and
check for gaps. Compare with the firmware rate: one reading per `SWEEP_DWELL_MS` plus
the sonar time, about 25-30 readings/s.

**Pass:** at least 20 readings/s on average, with no gap longer than 500 ms during a
full sweep.

## 4. Demo-mode fallback (owner: web)

**Goal:** the page falls back to demo data when the ESP32 is absent, and switches to
live data when it is present.

**Procedure:** open `index.html` with the ESP32 off. Status should say "demo". Power
the ESP32 and wait up to 3 s. Status should say "live".

**Pass:** both transitions happen without a page reload.

## Log template

```
Date:
Tester:
Test:
Setup (distances, angles, firmware commit):
Results (median, spread, pass/fail):
Notes:
```
