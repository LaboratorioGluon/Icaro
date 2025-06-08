# Sensor Board

Board responsible for collecting data from sensors and telemetry.

## Functionality

## Main Components:

| PCB | Interface | Description |
| --- | --------- | ----------- |
| ESP32-WROOM-32D/E | All | Main microcontroller |
| BMI160 | SPI/I2C `(TBD)` | IMU |
| BME280 | I2C | Humidity and temperature sensor |
| `(TBD)` | `(TBD)` | PT100/PT1000 reader |

| External | Connector | Interface | Description |
|----------|-----------|-----------|-------------|
| GPS | jst-ph? | UART | GPS compatible with both cheap and expensive models |
| __(x2)__ PT100/PT1000 | jst-ph? | Analog | Temperature sensors |

External connector:
- 3-pin jst-ph (2mm)
- 4-pin jst-ph? for GPS

## Design
