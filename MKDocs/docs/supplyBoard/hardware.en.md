# Supply Board

Board responsible for adapting and distributing power to the rest of the system.

## Features

The Supply Board must be capable of generating 5V and 3.3V from a 1S/2S battery voltage, as well as being able to turn outputs on/off and measure power consumption on each of its outputs.

## Main Components

- Processor: `STM32L031`
- Buck-Boost: `TPS63070`
- Buck converter for 3.3V: `TPS564257`

## Specifications

## Design

![](SupplyBoard.png)

## Interfaces

### Connectors

| ID | Name | Voltage | Current | Connector | Description |
|-|-| :--: | :--: | :--: | - |
| `J1` | `CI_BATT` | `3.3 V - 15.0 V` | `2 A` | XT-30 | Battery input connector. |
| `J2` | `CI_USB` | `5 V` | `300 mA` | micro-USB | USB charging connector. |
| `J3` | `CIO_MAIN_CONNECTOR`| `-` | `-` | 02x08 H2.0 | Main connector to the rest of the boards. |

### Power Supplies

| Name | Voltage | Current | Interface | Description |
|-| :--: | :--: | :--: | - |
| `VI_BATT` | `3.3 V - 15.0 V` | `2 A` | `CI_BATT` | Battery input connector. |
| `VI_USB` | `5 V` | `300 mA` | `CI_USB` | USB charging connector. |
| `VO_5V` | `5 V` | `1000 mA`| `CIO_MAIN_CONNECTOR` | 5V output |
| `VO_3V3` | `3.3 V` | `1000 mA`| `CIO_MAIN_CONNECTOR` | 3.3V output |
| `VO_BYPASS` | Same as `VI_BATT` | `2 A` | `CIO_MAIN_CONNECTOR` | Power bypass |

### Signals 

| Name | Voltage | Interface | Description |
|-| :--: | :-- | :-- |
| `CONTROL_ONOFF_BYPASS` | `3.3 V` | `CIO_MAIN_CONNECTOR` | Control signal to turn the `VO_BYPASS` output on and off. |
| `CONTROL_ONOFF_5V` | `3.3 V` |  `CIO_MAIN_CONNECTOR` | Control signal to turn the `VO_5V` output on and off. |
| `CURRENT_SENSI_BATT` | `Variable` | `CIO_MAIN_CONNECTOR` | Voltage dependent on the current **entering** via `VI_BATT`. |
| `CURRENT_SENSE_BYPASS` | `Variable` | `CIO_MAIN_CONNECTOR` | Voltage dependent on the current **exiting** via `VO_BYPASS`. |
| `CURRENT_SENSE_5V` | `Variable` | `CIO_MAIN_CONNECTOR` | Voltage dependent on the current **exiting** via `VO_5V`. |
| `CURRENT_SENSE_3V3` | `Variable` | `CIO_MAIN_CONNECTOR` | Voltage dependent on the current **exiting** via `VO_3V3`. |
