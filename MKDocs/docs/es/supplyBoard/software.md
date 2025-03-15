# Supply Board - Software

## Pinout

Configuración de los pines para el STM32L031.

| Pin | Modo | Alt.Función | Desc. |
|--|--|--|--|
|**PUERTO A**| | | |
| PA0 | `ANALOG`        | -         | Voltaje V_IN          |
| PA1 | `ANALOG`        | -         | Voltaje 5V            |
| PA2 | `ANALOG`        | -         | Corriente 3v3         |
| PA3 | `ANALOG`        | -         | Corriente 5v          |
| PA4 | `ANALOG`        | -         | Corriente IN          |
| PA5 | `ANALOG`        | -         | Corriente BYPASS      |
| PA6 | `ANALOG`        | -         | Sensor Temperatura    |
| PA7 | `ANALOG`        | -         | Sensor Temperatura    |
| PA8 | `ANALOG`        | -         | Sensor Temperatura    |
| PA9 | `AF_PP`         | `TBD`     | I2C SCL               |
| PA10| `AF_PP`         | `TBD`     | I2C SDA               |
|**PUERTO B**| ||
| PB6 | `AF_PP`         | `USART2`  | UART                  |
| PB7 | `AF_PP`         | `USART2`  | UART                  |
|**PUERTO C**| ||
| PC14 | `OUTPUT_PP`    | -         | Enable de BYPASS      |
| PC15 | `OUTPUT_PP`    | -         | Enable de 5V          |

## Periféricos

### UART
 - **BaudRate**: 115200
 - **Parity**: None
 - **Data Length**: 8B
 - **Stop bits**: 1


### I2C
 - **Mode**: Slave
 - **Addr**: 0x15? 
 - **Clock**: 400 KHz

 ## Mensajes
