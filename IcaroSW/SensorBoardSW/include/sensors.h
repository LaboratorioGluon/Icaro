#ifndef SENSORS_H__
#define SENSORS_H__


#include <freertos/FreeRTOS.h>
#include "bme280.h"
#include "classBmi160.h"
#include "bz251.h"

extern "C" {
    #include "ADS1115.h"
}

#define PT100_1_VB_CAL 0.21f
#define PT100_2_VB_CAL 0.21f

typedef struct{
    int64_t timestamp; // [ms]
    bme280_data bme;
    Bmi160::Data acc; // [m/s^2]
    Bmi160::Data gyr; // [deg/s]
    Bz251Data gps;  
    float pt100_1; // [C]
    float pt100_2; // [C]
    float vin; // [V]
    float v5voltage; // [V]
    float v3v3current; // [A]
    float v5current; // [A]
} SensorData;




extern SensorData sensorData;

extern Bmi160 bmi160;
extern bme280_dev bme280;
extern Bz251 bz251;

void sensors_init();

float sensors_pt100RtoT(float R);

float sensors_AdsToTemp(uint16_t val, float gainFsr, float vb, float vcc, float r1);
void sensors_getPt100(float &pt100_1, float &pt100_2);


void debugSensorData();


#endif // SENSORS_H__