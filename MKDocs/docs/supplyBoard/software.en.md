# Supply Board - Software

## Pinout

Pin configuration for the STM32L031.

| Pin | Mode | Alt. Function | Description |
|--|--|--|--|
|**PORT A**| | | |
| PA0 | `ANALOG`        | -         | V_IN Voltage          |
| PA1 | `ANALOG`        | -         | 5V Voltage            |
| PA2 | `ANALOG`        | -         | 3.3V Current         |
| PA3 | `ANALOG`        | -         | 5V Current          |
| PA4 | `ANALOG`        | -         | IN Current          |
| PA5 | `ANALOG`        | -         | BYPASS Current      |
| PA6 | `ANALOG`        | -         | Temperature Sensor    |
| PA7 | `ANALOG`        | -         | Temperature Sensor    |
| PA8 | `ANALOG`        | -         | Temperature Sensor    |
| PA9 | `AF_PP`         | `TBD`     | I2C SCL               |
| PA10| `AF_PP`         | `TBD`     | I2C SDA               |
|**PORT B**| |||
| PB6 | `AF_PP`         | `USART2`  | UART                  |
| PB7 | `AF_PP`         | `USART2`  | UART                  |
|**PORT C**| |||
| PC14 | `OUTPUT_PP`    | -         | Enable BYPASS      |
| PC15 | `OUTPUT_PP`    | -         | Enable 5V          |

## Peripherals

### UART
 - **BaudRate**: 115200
 - **Parity**: None
 - **Data Length**: 8B
 - **Stop bits**: 1

### I2C
 - **Mode**: Slave
 - **Addr**: 0x15? 
 - **Clock**: 400 KHz

## Messages
