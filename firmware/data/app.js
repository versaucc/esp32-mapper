// Sonar Mapper UI: draws live readings from the ESP32 on a polar map.
// Connects to ws://<host>/ws. If no ESP32 answers (or the page is opened
// from disk), it falls back to demo mode and generates fake readings.
(() => {
  "use strict";

  // ---- settings ----
  const MAX_CM = 400;          // outer ring of the map (matches SONAR_MAX_CM)
  const RING_STEP_CM = 100;
  const FADE_MS = 4000;        // how long a point stays visible
  const MAX_POINTS = 3000;
  const RECONNECT_MS = 3000;
  const DEMO_AFTER_MS = 2000;  // wait this long for a WebSocket before demo starts
  const DEMO_STEP_MS = 30;     // matches SWEEP_DWELL_MS in firmware/include/config.h
  const DEMO_STEP_DEG = 2;     // matches SWEEP_STEP_DEG in firmware/include/config.h

  // ---- DOM ----
  const canvas = document.getElementById("radar");
  const ctx = canvas.getContext("2d");
  const statusEl = document.getElementById("status");
  const angleEl = document.getElementById("r-angle");
  const distEl = document.getElementById("r-dist");
  const demoBtn = document.getElementById("demo-toggle");

  // ---- state ----
  const points = [];           // { angle, dist, seen } where seen = performance.now()
  let sweepAngle = 0;
  let socket = null;
  let retryTimer = null;
  let demoTimer = null;
  let forcedDemo = new URLSearchParams(location.search).has("demo");

  // ---- geometry: angle 0 = left edge, 180 = right edge, arc opens upward ----
  const cx = () => canvas.width / 2;
  const cy = () => canvas.height - 20;
  const radius = () => cy() - 20;

  function polarToXY(angleDeg, distCm) {
    const r = (distCm / MAX_CM) * radius();
    const theta = Math.PI * (1 - angleDeg / 180);
    return [cx() + r * Math.cos(theta), cy() - r * Math.sin(theta)];
  }

  // ---- drawing ----
  function drawGrid() {
    ctx.strokeStyle = "#30363d";
    ctx.fillStyle = "#8b949e";
    ctx.lineWidth = 1;
    ctx.font = "12px system-ui, sans-serif";

    for (let d = RING_STEP_CM; d <= MAX_CM; d += RING_STEP_CM) {
      const r = (d / MAX_CM) * radius();
      ctx.beginPath();
      ctx.arc(cx(), cy(), r, Math.PI, 2 * Math.PI);
      ctx.stroke();
      ctx.fillText(`${d} cm`, cx() + 4, cy() - r - 2);
    }

    for (let a = 0; a <= 180; a += 30) {
      const [x, y] = polarToXY(a, MAX_CM);
      ctx.beginPath();
      ctx.moveTo(cx(), cy());
      ctx.lineTo(x, y);
      ctx.stroke();
      ctx.fillText(`${a}°`, x + (a === 0 ? -28 : a === 180 ? 6 : 0), y - 6);
    }
  }

  function drawPoints(now) {
    // drop expired points from the front
    while (points.length && now - points[0].seen > FADE_MS) {
      points.shift();
    }
    for (const p of points) {
      const alpha = 1 - (now - p.seen) / FADE_MS;
      const [x, y] = polarToXY(p.angle, p.dist);
      ctx.fillStyle = `rgba(63, 185, 80, ${alpha.toFixed(3)})`;
      ctx.fillRect(x - 2, y - 2, 4, 4);
    }
  }

  function drawSweepLine() {
    const [x, y] = polarToXY(sweepAngle, MAX_CM);
    ctx.strokeStyle = "rgba(63, 185, 80, 0.6)";
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(cx(), cy());
    ctx.lineTo(x, y);
    ctx.stroke();
  }

  function render() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGrid();
    drawPoints(performance.now());
    drawSweepLine();
    requestAnimationFrame(render);
  }

  // ---- data handling ----
  function handleReading(msg) {
    if (typeof msg.angle !== "number" || typeof msg.dist_cm !== "number") return;

    sweepAngle = msg.angle;
    angleEl.textContent = `${msg.angle}°`;

    if (msg.dist_cm < 0) {
      distEl.textContent = "no echo";
      return;
    }
    distEl.textContent = `${msg.dist_cm} cm`;
    if (msg.dist_cm <= MAX_CM) {
      points.push({ angle: msg.angle, dist: msg.dist_cm, seen: performance.now() });
      if (points.length > MAX_POINTS) points.shift();
    }
  }

  // ---- status ----
  function setStatus(text, live = false) {
    statusEl.textContent = text;
    statusEl.classList.toggle("live", live);
  }

  // ---- demo mode: fake sweep with a wall and one object ----
  let demoAngle = 0;
  let demoDir = 1;
  function demoTick() {
    demoAngle += demoDir * DEMO_STEP_DEG;
    if (demoAngle >= 180) { demoAngle = 180; demoDir = -1; }
    if (demoAngle <= 0) { demoAngle = 0; demoDir = 1; }

    let dist = 150 + 40 * Math.sin((demoAngle * Math.PI) / 45);   // curved wall
    if (demoAngle >= 60 && demoAngle <= 75) dist = 70;            // "object"
    dist += (Math.random() - 0.5) * 4;                            // sensor noise
    if (Math.random() < 0.03) dist = -1;                          // occasional dropout

    handleReading({ angle: demoAngle, dist_cm: Math.round(dist), t: Math.round(performance.now()) });
  }

  function startDemo() {
    if (demoTimer) return;
    demoTimer = setInterval(demoTick, DEMO_STEP_MS);
    setStatus("demo (no ESP32)");
  }

  function stopDemo() {
    if (!demoTimer) return;
    clearInterval(demoTimer);
    demoTimer = null;
  }

  // ---- WebSocket connection ----
  function stopSocket() {
    clearTimeout(retryTimer);
    retryTimer = null;
    if (socket) {
      socket.onopen = socket.onmessage = socket.onclose = null;
      socket.close();
      socket = null;
    }
  }

  function scheduleRetry() {
    clearTimeout(retryTimer);
    retryTimer = setTimeout(connect, RECONNECT_MS);
  }

  function connect() {
    if (forcedDemo) return;

    // Opened from disk (file://): there is no ESP32 to reach, so use demo data.
    if (location.protocol === "file:") {
      startDemo();
      setStatus("demo (opened from file)");
      return;
    }

    setStatus("connecting…");
    const url = `ws://${location.host}/ws`;
    let ws;
    try {
      ws = new WebSocket(url);
    } catch (err) {
      startDemo();
      scheduleRetry();
      return;
    }
    socket = ws;

    let opened = false;
    const fallback = setTimeout(() => {
      if (!opened) startDemo();
    }, DEMO_AFTER_MS);

    ws.onopen = () => {
      opened = true;
      clearTimeout(fallback);
      stopDemo();
      setStatus("live", true);
    };

    ws.onmessage = (ev) => {
      try {
        handleReading(JSON.parse(ev.data));
      } catch (err) {
        console.warn("bad message", ev.data);
      }
    };

    ws.onclose = () => {
      clearTimeout(fallback);
      socket = null;
      startDemo();
      scheduleRetry();
    };
  }

  // ---- demo button: force demo on/off ----
  function updateDemoButton() {
    demoBtn.textContent = forcedDemo ? "Stop demo" : "Demo mode";
  }

  demoBtn.addEventListener("click", () => {
    forcedDemo = !forcedDemo;
    updateDemoButton();
    if (forcedDemo) {
      stopSocket();
      startDemo();
      setStatus("demo (forced)");
    } else {
      stopDemo();
      connect();
    }
  });

  // ---- start ----
  updateDemoButton();
  if (forcedDemo) {
    startDemo();
    setStatus("demo (forced)");
  } else {
    connect();
  }
  requestAnimationFrame(render);
})();
