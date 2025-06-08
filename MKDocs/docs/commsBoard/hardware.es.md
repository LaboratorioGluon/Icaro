# CommsBoard Hardware

## Componentes

- E22-400M30S: [Aliexpress](https://s.click.aliexpress.com/e/_oBgQFeH)


## Contenido mensajes

- Timestamp
    - Fecha  (4B)
    - Hora   (2B)
- Posicion:
    - Lat   (4B)
    - Lon   (4B)
    - Alt   (4B)
    - Vel H/V (GPS) (4B + 4B)
    - Num. Satélites (1B)
    - Alt.Baro (4B)
- Actitud:
    - Acc X,Y,Z (4B*3)
    - Gyr X,Y,Z (4B*3)
- Sensores:
    - T.Exterior (2B)
    - T.Interior (2B)
    - T.PCB      (2B)
    - Presión    (4B)
- Supply:
    - V. Batería    (4B)
    - V. 5v         (4B)
    - C. Batería    (4B)
    - C. 5v         (4B)
    - C. 3v3        (4B)

85 bytes

### Kaitai