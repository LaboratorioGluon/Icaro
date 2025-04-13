# Interfaz de la SupplyBoard

## Mapa de registros I2C

| Offset | Reset | Tamaño (bytes) | Nombre | Descripción |
|:--:|-|-|-|-|
| 0x0 | 0x01 | 1 | ID     | Código de identificación |
| 0x1 | 0x00 | 1 | STATUS | Registro de estado |
| 0x2 | 0x00 | 2 | V5     | Voltaje de la línea 5v en mV VVVmm |
| 0x4 | 0x00 | 2 | VBATT  | Voltaje de la línea 5v en mV VVVmm |
| 0x6 | 0x00 | 2 | I33    | Ammmu  (Poner factor) |
| 0x8 | 0x00 | 2 | I5     | Ammmu|
| 0xA| 0x00 | 2 | TEMP   | Valor + 100 |
