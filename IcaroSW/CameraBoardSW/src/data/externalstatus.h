#pragma once

#include <stdint.h>

namespace Data
{

    typedef struct
    {
        uint64_t externalTS;
        uint8_t sensorBoardStatus;
        uint8_t commsBoardStatus;
        uint8_t powerBoardStatus;
    } ExternalBoardsStatus_t;

    typedef struct
    {
        uint64_t externalTS;
        uint8_t status3v3;
        uint8_t status5v;
        float batteryLevel;
    } ExternalPowerStatus_t;

    typedef struct
    {
        uint64_t externalTS;
        float InternalTemp;
        float ExternalTemp;
        float OnboardTemp;
        float Humidity;
    } ExternalSensorsStatus_t;

    typedef struct
    {
        uint64_t externalTS;
        float accelX;
        float accelY;
        float accelZ;
    } ExternalAccelStatus_t;

    typedef struct
    {
        uint64_t externalTS;
        float gyroX;
        float gyroY;
        float gyroZ;
    } ExternalGyroStatus_t;
        
    typedef struct
    {
        uint64_t externalTS;
        float latitude;
        float longitude;
        float altitude;
    } ExternalGPSStatus_t;

    typedef struct
    {
        uint64_t boardTS;
        uint64_t externalTS;
        uint16_t year;
        uint8_t  month;
        uint8_t  day;
        uint8_t  hour;
        uint8_t  minute;
        uint8_t  second;
        uint16_t millisecond;
    } ExternalTimeStatus_t;

    typedef struct
    {
        ExternalBoardsStatus_t  boards;
        ExternalPowerStatus_t   power;
        ExternalSensorsStatus_t sensors;
        ExternalAccelStatus_t   accel;
        ExternalGyroStatus_t    gyro;
        ExternalGPSStatus_t     gps;
        ExternalTimeStatus_t    time;
    } ExternalStatus_t;

}
