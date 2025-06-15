meta:
  id: icaro
  endian: le
seq:
  - id: timestamp
    type: s8
  - id: bme280
    type: bme
  - id: acc
    type: bmi
  - id: gyro
    type: bmi
  - id: gps
    type: gps
  - id: pt100_ext
    type: f4
  - id: pt100_int
    type: f4
  - id: vin
    type: u2
  - id: v5
    type: u2
  - id: currentv3
    type: u2
  - id: currentv5
    type: u2 
  - id: supplyboardtemp
    type: u2
  - id: supplymcutemp
    type: u2
  - id: increment
    type: u2

types:
  bme:
    seq:
      - id: pressure
        type: f8
      - id: temperature
        type: f8
      - id: humidity
        type: f8
  bmi:
    seq:
      - id: x
        type: f4
      - id: y
        type: f4
      - id: z
        type: f4
      - id: time
        type: f4
  gps:
    seq:
      - id: valid
        type: u1
      - id: year
        type: u1
      - id : month
        type: u1
      - id: day
        type: u1
      - id: latitude
        type: f4
      - id: longitude
        type: f4
      - id: altitude
        type: f4
      - id: hour
        type: u1
      - id: minute
        type: u1
      - id: seconds
        type: u1
      - id: satellite
        type: u1
      - id: speed
        type: f4