# sonar-mapper

An ESP32 sweeps an RCWL-1601 ultrasonic sensor across 180° on a servo and streams
each `angle` / `distance` reading over Wi-Fi (WebSocket). A webpage served by the
ESP32 draws the readings as a live polar map.

ECE341E junior design project.

## How it works

```
RCWL-1601 --(trig/echo)--> ESP32 --(Wi-Fi, WebSocket /ws)--> browser polar map
                             |
                    servo PWM (0 -> 180 -> 0)
```

Each reading is sent as JSON:

```json
{"angle": 90, "dist_cm": 142, "t": 123456}
```

`dist_cm` is `-1` when there is no echo or the object is out of range.

## Hardware

| Part | Connection (see `firmware/include/config.h`) |
| --- | --- |
| RCWL-1601 | VCC to 3.3 V, GND, TRIG to GPIO 26, ECHO to GPIO 27 |
| Servo | Signal to GPIO 18, plus its own power and common ground |
| ESP32 dev board | Any ESP32 DevKit-style board (`esp32dev`) |

Share ground between the ESP32, sensor and servo.

## Setup

1. Install [VS Code](https://code.visualstudio.com/) with the PlatformIO extension,
   or install the CLI: `pip install platformio`.
2. Clone the repo and open the folder.
3. Edit `firmware/include/config.h` for your Wi-Fi settings and pins.
   Keep real passwords out of commits. Use a private network for testing if you can.

## Build, upload, run

```bash
pio run                  # build firmware
pio run -t upload        # flash firmware to the ESP32
pio run -t uploadfs      # flash the webpage in firmware/data/ (run after any change there)
pio device monitor       # serial log at 115200 baud
```

With `WIFI_USE_AP 1` (default):

1. Connect a laptop or phone to the Wi-Fi network named by `WIFI_AP_SSID`.
2. Open `http://192.168.4.1` in a browser.

With `WIFI_USE_AP 0`, the ESP32 joins `WIFI_STA_SSID`; find its IP in the serial monitor.

No hardware? Open `firmware/data/index.html` directly for demo mode. See
[web-prototype/README.md](web-prototype/README.md).

## Folder map

```
platformio.ini          PlatformIO build config (env: esp32dev)
firmware/
  include/config.h      all pins, sweep settings, Wi-Fi credentials
  src/main.cpp          setup/loop, wires the modules together
  src/sonar.{h,cpp}     RCWL-1601 trigger/echo -> distance in cm
  src/servo_sweep.{h,cpp} servo 0-180-0 sweep with step and dwell
  src/net.{h,cpp}       Wi-Fi, web server, LittleFS, /ws broadcast
  data/                 webpage served from LittleFS (index.html, app.js, style.css)
web-prototype/          how to run the UI without hardware
hardware/kicad/         schematics and PCB (placeholder)
mechanical/             sensor/servo mount designs (placeholder)
docs/                   meeting notes, AI-use log, block diagram
tests/                  bench test plan and logs
.github/                PR template, task issue template, CI workflow (pio run)
```

## Team

Work is split by subsystem: `sonar`, `servo`, `net`, `web`, `hardware`, `mechanical`, `docs`, `tests`.
Unfinished code is marked `TODO(<subsystem>)`. See [CONTRIBUTING.md](CONTRIBUTING.md) for the branch and review workflow.

## License

MIT. See [LICENSE](LICENSE).
