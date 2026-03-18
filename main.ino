/*
  Archimedes-1 | main.ino
  -----------------------
  Hull environment monitoring for the Archimedes-1 submarine.

  Hardware:
    - Seeed XIAO ESP32S3 Sense
    - Adafruit BME280 (I2C / STEMMA QT) — internal hull sensor

  What it does:
    1. Reads temperature, humidity, and pressure from the BME280
    2. Hosts a WiFi Access Point named "Archimedes-1"
    3. Serves live readings as JSON at  http://192.168.4.1/hull
    4. Serves a live HTML dashboard at  http://192.168.4.1/

  To view telemetry:
    1. Connect your laptop/phone to WiFi: "Archimedes-1" (pw: submarine)
    2. Open browser: http://192.168.4.1

  Libraries required (Arduino Library Manager):
    - Adafruit BME280 Library
    - Adafruit Unified Sensor

  Board: XIAO_ESP32S3 (Seeed ESP32 board package)
*/

#include "sensors/bme280.h"
#include "comms/wifi_sender.h"

#define SERIAL_BAUD       115200
#define SENSOR_INTERVAL   2000    // ms between sensor reads & Serial prints

static unsigned long _lastRead = 0;

// --------------------------------------------------------------------------

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) delay(10);

  Serial.println("=================================");
  Serial.println("  ARCHIMEDES-1 — Hull Monitor   ");
  Serial.println("=================================");

  bool sensorOk = bme280_init();
  if (!sensorOk) {
    Serial.println("[MAIN] WARNING: BME280 failed to init. Continuing anyway.");
  }

  wifi_init();

  Serial.println("[MAIN] Setup complete. Running.");
  Serial.println("---------------------------------");
}

// --------------------------------------------------------------------------

void loop() {
  // Handle incoming HTTP requests continuously
  wifi_handle_clients();

  // Print sensor data to Serial at the defined interval
  unsigned long now = millis();
  if (now - _lastRead >= SENSOR_INTERVAL) {
    _lastRead = now;

    HullEnvironment env = bme280_read();

    if (env.valid) {
      Serial.printf("[HULL] Temp: %.1f°C / %.1f°F  |  Humidity: %.1f%%  |  Pressure: %.2f hPa\n",
        env.temperatureC, env.temperatureF, env.humidity, env.pressureHPa);
    } else {
      Serial.println("[HULL] Sensor read failed.");
    }
  }
}
