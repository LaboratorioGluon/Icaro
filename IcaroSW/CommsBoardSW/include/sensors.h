#ifndef SENSORS_H__
#define SENSORS_H__

#include <stdint.h>


typedef struct {
    uint8_t valid;
    uint8_t year;       /* yy */
    uint8_t month;      /* mm */
    uint8_t day;        /* dd */
    float latitude;     /* dd.mmmmmm */
    float longitude;    /* dd.mmmmmm */
    float altitude;     /* Antenna altitude above/below mean sea level (meters) */
    uint8_t hour;       /* hh */
    uint8_t minute;     /* mm */
    uint8_t satellites; /* Number of satellites in use. May be different to the number in view */
    uint8_t dummy;
    float speedKmh;     /* Speed over ground, kms */
} Bz251Data;

typedef struct{
        float x;
        float y;
        float z;
        float time;
    } Bmi160_Data;


struct bme280_data
{
    /*! Compensated pressure */
    double pressure;

    /*! Compensated temperature */
    double temperature;

    /*! Compensated humidity */
    double humidity;
};

typedef struct{
    int64_t timestamp; // [ms]
    bme280_data bme;
    Bmi160_Data acc; // [m/s^2]
    Bmi160_Data gyr; // [deg/s]
    Bz251Data gps;  
    float pt100_1; // [C]
    float pt100_2; // [C]
    float vin; // [V]
    float v5voltage; // [V]
    float v3v3current; // [A]
    float v5current; // [A]
    uint16_t supplyTemp;
} SensorDataLora;


#endif // SENSORS_H__