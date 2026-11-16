# Web prototype: UI work without hardware

Use this to change the webpage in `firmware/data/` without an ESP32 or sensor.

## Open the page

1. Open `firmware/data/index.html` in a browser. Double-clicking the file works.
2. The page sees the `file://` protocol, so it skips the WebSocket and starts
   **demo mode** automatically. Status shows `demo (opened from file)`.
3. The demo sweeps 0° to 180° and back, with a curved wall at about 150 cm, an
   "object" near 70 cm at 60-75°, and occasional dropouts.

You can also run it with a local server to get the same behavior and test the
`?demo` URL:

```bash
cd firmware/data
python3 -m http.server 8000
# open http://localhost:8000/index.html?demo
```

## Demo controls

- `?demo` in the URL starts demo mode immediately and does not try to connect.
- The **Demo mode** button toggles forced demo on and off. Turning it off reconnects to the ESP32.
- When the page is served by the ESP32 and no WebSocket opens within 2 s, demo mode starts
  on its own. It stops when live data arrives.

## Demo data format

The demo emits the same JSON as the firmware:

```json
{"angle": 90, "dist_cm": 148, "t": 123456}
```

`dist_cm: -1` means "no echo" and is not plotted. Keep the demo in sync with
`firmware/include/config.h` (`SWEEP_STEP_DEG`, `SWEEP_DWELL_MS`).

## Testing changes

- Check the page at phone width (about 390 px) as well as desktop.
- Check the layout in dark mode; the page is dark by default.
- After a change to `firmware/data/`, run `pio run -t uploadfs` to update the ESP32.
