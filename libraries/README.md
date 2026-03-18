# Libraries

This folder documents all external Arduino libraries required to build Archimedes-1.

---

## Required Libraries

### 1. Adafruit BME280 Library — v2.2.4
- **Purpose:** Driver for the BME280 temperature / humidity / pressure sensor inside the hull
- **Install:** Arduino IDE → Tools → Manage Libraries → search `Adafruit BME280`
- **Source:** https://github.com/adafruit/Adafruit_BME280_Library

### 2. Adafruit Unified Sensor — v1.1.15
- **Purpose:** Dependency of the BME280 library — standardises sensor data types
- **Install:** Arduino IDE → Tools → Manage Libraries → search `Adafruit Unified Sensor`
- **Source:** https://github.com/adafruit/Adafruit_Sensor

---

## Board Setup

You also need the **Seeed Studio ESP32 board package** installed in Arduino IDE:

1. Open Arduino IDE → File → Preferences
2. Add this URL to "Additional Boards Manager URLs":
   ```
   https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
   ```
3. Go to Tools → Board → Boards Manager → search `Seeed XIAO ESP32S3` → Install
4. Select board: **Tools → Board → XIAO_ESP32S3**

---

## Arduino CLI (alternative)

If you prefer the command line:

```bash
arduino-cli lib install "Adafruit BME280 Library"@2.2.4
arduino-cli lib install "Adafruit Unified Sensor"@1.1.15
```

---

## Notes

- The built-in `WiFi` and `WebServer` libraries ship with the ESP32 board package — no separate install needed.
- Always match the versions listed here to avoid API changes breaking the build.
