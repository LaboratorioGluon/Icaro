# Supply Board - Software

## Pinout

Pin configuration for the STM32L031.

| Pin  | Mode           | Alt. Function | Description               |
|------|----------------|----------------|---------------------------|
| **PORT A** |||
| PA0  | `ANALOG`       | -              | Voltage V_IN              |
| PA1  | `ANALOG`       | -              | Voltage 5V                |
| PA2  | `ANALOG`       | -              | Current 3.3V              |
| PA3  | `ANALOG`       | -              | Current 5V                |
| PA4  | `ANALOG`       | -              | Input Current             |
| PA5  | `ANALOG`       | -              | BYPASS Current            |
| PA6  | `ANALOG`       | -              | Temperature Sensor        |
| PA7  | `OUTPUT_PP`    | -              | CS0                       |
| PA8  | `OUTPUT_PP`    | -              | CS1                       |
| PA9  | `AF_PP`        | `TBD`          | I2C SCL                   |
| PA10 | `AF_PP`        | `TBD`          | I2C SDA                   |
| PA13 | ***DO NOT USE*** | -            | SWDIO                     |
| PA14 | ***DO NOT USE*** | -            | SWCLK                     |
| PA15 | `OUTPUT_PP`    | -              | Output for loop control   |
| **PORT B** |||
| PB0  | `AF_PP`        | `AF1_SPI1`     | SPI1 MISO                 |
| PB1  | `AF_PP`        | `AF0_SPI1`     | SPI1 MOSI                 |
| PB3  | `AF_PP`        | `AF0_SPI1`     | SPI1 SCK                  |
| PB6  | `AF_PP`        | `AF0_USART2`   | UART                      |
| PB7  | `AF_PP`        | `AF0_USART2`   | UART                      |
| **PORT C** |||
| PC14 | `OUTPUT_PP`    | -              | BYPASS Enable             |
| PC15 | `OUTPUT_PP`    | -              | 5V Enable                 |


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
