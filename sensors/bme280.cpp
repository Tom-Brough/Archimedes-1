/*
  sensors/bme280.cpp
  ------------------
  Implementation of hull environment monitoring.
*/

#include "bme280.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// I2C address: 0x76 (SDO->GND, default) or 0x77 (SDO->3.3V)
#define BME280_ADDRESS 0x76

static Adafruit_BME280 _bme;
static bool _initialised = false;

// --------------------------------------------------------------------------

bool bme280_init() {
  Wire.begin(); // Uses XIAO default SDA=D4, SCL=D5

  if (!_bme.begin(BME280_ADDRESS)) {
    Serial.println("[BME280] ERROR: Sensor not found. Check wiring/address.");
    return false;
  }

  // Optimised for indoor / slow-changing hull environment
  _bme.setSampling(
    Adafruit_BME280::MODE_NORMAL,
    Adafruit_BME280::SAMPLING_X2,    // Temperature
    Adafruit_BME280::SAMPLING_X16,   // Pressure
    Adafruit_BME280::SAMPLING_X1,    // Humidity
    Adafruit_BME280::FILTER_X16,
    Adafruit_BME280::STANDBY_MS_500
  );

  _initialised = true;
  Serial.println("[BME280] Initialised OK.");
  return true;
}

// --------------------------------------------------------------------------

HullEnvironment bme280_read() {
  HullEnvironment env;

  if (!_initialised) {
    env.valid = false;
    return env;
  }

  env.temperatureC = _bme.readTemperature();
  env.temperatureF = env.temperatureC * 9.0 / 5.0 + 32.0;
  env.humidity     = _bme.readHumidity();
  env.pressureHPa  = _bme.readPressure() / 100.0F;

  // Basic sanity check — if pressure is 0 the sensor likely failed
  env.valid = (env.pressureHPa > 0.0);

  return env;
}
