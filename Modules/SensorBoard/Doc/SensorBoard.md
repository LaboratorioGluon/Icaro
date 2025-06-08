# Sensor Board

Tarjeta encargada de recoger datos de los sensores y telemetría

## Funcionalidad


## Componentes principales:

|PCB | Interfaz | Desc. |
| --- | --- | --- |
| ESP32-WROOM-32D/E | Todos | Microcontrolador principal |
| BMI160 | SPI/I2C `(TBD)` | IMU |
| BME280 | I2C | Sensor humedad y temperatura |
| `(TBD)` | `(TBD)` | Lector PT100/PT1000 |


| Externo | Conector | Interfaz | Desc. |
|---|---|---|---|
| GPS | jst-ph? | UART | GPS compatible con los GPS baratos y caros. |
| __(x2)__ PT100/PT1000 | jst-ph? | Analogico | Sensores de temperatura | 


Conector externo:
- 3-pin jst-ph (2mm):
- 4pin jst-ph? GPS

## Diseño
