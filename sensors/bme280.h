#pragma once

/*
  sensors/bme280.h
  ----------------
  Hull environment monitoring via Adafruit BME280 (I2C / STEMMA QT).
  Reads internal temperature, humidity, and pressure.

  Wiring (XIAO ESP32S3 Sense):
    BME280 VIN  -> 3.3V
    BME280 GND  -> GND
    BME280 SCL  -> D5 (GPIO7)
    BME280 SDA  -> D4 (GPIO6)
*/

#include <Arduino.h>

struct HullEnvironment {
  float temperatureC;
  float temperatureF;
  float humidity;
  float pressureHPa;
  bool  valid;       // false if sensor read failed
};

bool            bme280_init();
HullEnvironment bme280_read();
