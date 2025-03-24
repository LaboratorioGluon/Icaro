# Sensor Board

Board in charge of sensor data collection and telemetry.

## Functionality


## Main components:

|PCB | Interface | Download | |
| --- | --- | --- |
| ESP32-WROOM-32D/E | All | | Main Microcontroller | |
| | BMI160 | SPI/I2C `(TBD)` | | IMU | |
| BME280 | I2C | I2C | Humidity and Temperature Sensor |
| `(TBD)` | `(TBD)` | PT100/PT1000 Reader |


| | External | Connector | Interface | Download | |
|---|---|---|---|
| GPS | jst-ph? | UART | GPS compatible with cheap and expensive GPS. |
|(x2)__ PT100/PT1000 | jst-ph? | Analogue | Temperature sensors |


External connector:
- 3-pin jst-ph (2mm):
- 4-pin jst-ph? GPS

## Design

Translated with DeepL.com (free version)
