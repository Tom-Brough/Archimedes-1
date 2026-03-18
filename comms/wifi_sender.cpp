/*
  comms/wifi_sender.h
  -------------------
  ESP32 Access Point + HTTP server for live hull telemetry.
*/

#include "wifi_sender.h"
#include <WiFi.h>
#include <WebServer.h>

static WebServer _server(HTTP_PORT);

// --------------------------------------------------------------------------
// HTML dashboard — auto-refreshes every 2 seconds
// --------------------------------------------------------------------------

static const char DASHBOARD_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Archimedes-1 | Hull Monitor</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: 'Segoe UI', sans-serif; background: #0a1628; color: #e0f0ff; min-height: 100vh; display: flex; flex-direction: column; align-items: center; justify-content: center; }
    h1 { font-size: 1.6rem; letter-spacing: 2px; color: #4fc3f7; margin-bottom: 4px; }
    .subtitle { font-size: 0.8rem; color: #546e8a; margin-bottom: 32px; letter-spacing: 1px; }
    .grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 16px; width: 100%; max-width: 480px; }
    .card { background: #0d2137; border: 1px solid #1e4060; border-radius: 12px; padding: 20px; text-align: center; }
    .card .label { font-size: 0.7rem; letter-spacing: 1.5px; text-transform: uppercase; color: #546e8a; margin-bottom: 8px; }
    .card .value { font-size: 2rem; font-weight: 700; color: #4fc3f7; }
    .card .unit  { font-size: 0.85rem; color: #546e8a; margin-top: 2px; }
    .status { margin-top: 24px; font-size: 0.75rem; color: #2e5070; }
    .status span { color: #4fc3f7; }
    #alert { display:none; margin-top:16px; padding:10px 20px; background:#7f1d1d; border:1px solid #ef4444; border-radius:8px; color:#fca5a5; font-size:0.85rem; }
  </style>
</head>
<body>
  <h1>⚓ ARCHIMEDES-1</h1>
  <div class="subtitle">HULL ENVIRONMENT MONITOR</div>

  <div class="grid">
    <div class="card">
      <div class="label">Temperature</div>
      <div class="value" id="tempC">--</div>
      <div class="unit">°C</div>
    </div>
    <div class="card">
      <div class="label">Temperature</div>
      <div class="value" id="tempF">--</div>
      <div class="unit">°F</div>
    </div>
    <div class="card">
      <div class="label">Humidity</div>
      <div class="value" id="humidity">--</div>
      <div class="unit">%</div>
    </div>
    <div class="card">
      <div class="label">Pressure</div>
      <div class="value" id="pressure">--</div>
      <div class="unit">hPa</div>
    </div>
  </div>

  <div id="alert">⚠ Sensor read failed — check BME280 connection</div>

  <div class="status">Last updated: <span id="ts">--</span></div>

  <script>
    async function fetchData() {
      try {
        const res = await fetch('/hull');
        const d = await res.json();
        if (d.valid) {
          document.getElementById('tempC').textContent    = d.temperature_c.toFixed(1);
          document.getElementById('tempF').textContent    = d.temperature_f.toFixed(1);
          document.getElementById('humidity').textContent = d.humidity.toFixed(1);
          document.getElementById('pressure').textContent = d.pressure_hpa.toFixed(1);
          document.getElementById('alert').style.display  = 'none';
        } else {
          document.getElementById('alert').style.display  = 'block';
        }
        document.getElementById('ts').textContent = new Date().toLocaleTimeString();
      } catch(e) {
        document.getElementById('alert').style.display = 'block';
      }
    }
    fetchData();
    setInterval(fetchData, 2000); // Poll every 2 seconds
  </script>
</body>
</html>
)rawhtml";

// --------------------------------------------------------------------------
// Route handlers
// --------------------------------------------------------------------------

static void handle_root() {
  _server.send(200, "text/html", DASHBOARD_HTML);
}

static void handle_hull() {
  HullEnvironment env = bme280_read();

  String json = "{";
  json += "\"valid\":"         + String(env.valid ? "true" : "false") + ",";
  json += "\"temperature_c\":" + String(env.temperatureC, 2) + ",";
  json += "\"temperature_f\":" + String(env.temperatureF, 2) + ",";
  json += "\"humidity\":"      + String(env.humidity, 2)     + ",";
  json += "\"pressure_hpa\":"  + String(env.pressureHPa, 2);
  json += "}";

  _server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow cross-origin requests
  _server.send(200, "application/json", json);
}

static void handle_health() {
  _server.send(200, "application/json", "{\"status\":\"ok\",\"device\":\"Archimedes-1\"}");
}

static void handle_not_found() {
  _server.send(404, "application/json", "{\"error\":\"not found\"}");
}

// --------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------

void wifi_init() {
  Serial.println("[WiFi] Starting Access Point...");

  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("[WiFi] AP IP: ");
  Serial.println(ip);
  Serial.print("[WiFi] SSID: ");
  Serial.println(WIFI_AP_SSID);
  Serial.println("[WiFi] Dashboard: http://" + ip.toString() + "/");

  _server.on("/",       HTTP_GET, handle_root);
  _server.on("/hull",   HTTP_GET, handle_hull);
  _server.on("/health", HTTP_GET, handle_health);
  _server.onNotFound(handle_not_found);

  _server.begin();
  Serial.println("[WiFi] HTTP server started.");
}

void wifi_handle_clients() {
  _server.handleClient();
}
