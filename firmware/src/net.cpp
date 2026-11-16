#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <cstring>
#include "config.h"
#include "net.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

static void startWifi() {
#if WIFI_USE_AP
  WiFi.mode(WIFI_AP);
  // softAP wants a password of 8+ characters; nullptr makes the network open
  WiFi.softAP(WIFI_AP_SSID, strlen(WIFI_AP_PASS) > 0 ? WIFI_AP_PASS : nullptr);
  Serial.printf("Wi-Fi AP '%s' up, open http://%s\n", WIFI_AP_SSID,
                WiFi.softAPIP().toString().c_str());
#else
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_STA_TIMEOUT_MS) {
    delay(200);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Joined '%s', open http://%s\n", WIFI_STA_SSID,
                  WiFi.localIP().toString().c_str());
  } else {
    Serial.println("Wi-Fi join failed");
    // TODO(net): fall back to AP mode when the station join fails
  }
#endif
}

static void onWsEvent(AsyncWebSocket *, AsyncWebSocketClient *client, AwsEventType type,
                      void *, uint8_t *, size_t) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client %u connected\n", client->id());
  }
  // TODO(net): handle incoming client messages (e.g. start/stop sweep) if the UI needs them
}

void netInit() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed (run: pio run -t uploadfs)");
  }

  startWifi();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "not found");
  });
  server.begin();
}

void netPublish(int angleDeg, int distCm, unsigned long tMs) {
  ws.cleanupClients();
  if (ws.count() == 0) {
    return;  // nobody listening; skip the work
  }

  JsonDocument doc;
  doc["angle"] = angleDeg;
  doc["dist_cm"] = distCm;
  doc["t"] = tMs;

  String out;
  serializeJson(doc, out);
  ws.textAll(out);
}
