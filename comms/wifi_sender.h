#pragma once

/*
  comms/wifi_sender.h
  -------------------
  Hosts a lightweight HTTP server on the ESP32 that serves live
  hull environment data as JSON. The operator's laptop / ground
  station can poll http://<ESP32-IP>/hull for sensor readings.

  Access Point mode: ESP32 creates its own WiFi network so no
  router is required during field deployment.
*/

#include <Arduino.h>
#include "../sensors/bme280.h"

// --- WiFi Access Point credentials (change these!) ---
#define WIFI_AP_SSID     "Archimedes-1"
#define WIFI_AP_PASSWORD "submarine"     // Min 8 chars for WPA2
#define WIFI_AP_CHANNEL  1

// --- HTTP server port ---
#define HTTP_PORT 80

void wifi_init();
void wifi_handle_clients();

// Endpoint paths served:
//   GET /       -> Simple HTML dashboard
//   GET /hull   -> JSON sensor data
//   GET /health -> {"status":"ok"}
