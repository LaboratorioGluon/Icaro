#pragma once

#include "../../../Modules/Common/SwLib/IMS/src/ims_messages.h"

namespace InterBoards::CameraMessages
{

    struct IMS_PACK IMS_Status
    {
        uint8_t sensorBoardStatus;
        uint8_t commsBoardStatus;
        uint8_t powerBoardStatus;
    };

    struct IMS_PACK IMS_Power
    {
        uint8_t status3v3;
        uint8_t status5v;
        float batteryLevel;
    };

    struct IMS_PACK IMS_Sensors
    {
        float InternalTemp;
        float ExternalTemp;
        float OnboardTemp;
        float Humidity;
    };

    struct IMS_PACK IMS_AttitudeAccel
    {
        float accelX;
        float accelY;
        float accelZ;
    };

    struct IMS_PACK IMS_AttitudeGyro
    {
        float gyroX;
        float gyroY;
        float gyroZ;
    };
        
    struct IMS_PACK IMS_GPS
    {
        double latitude;
        double longitude;
        double altitude;
    };

    struct IMS_PACK IMS_DateTime
    {
        uint16_t year;
        uint8_t  month;
        uint8_t  day;
        uint8_t  hour;
        uint8_t  minute;
        uint8_t  second;
        uint16_t millisecond;
    };

    enum class IMS_TAGS
    {
        Raw = 0,
        Status,
        Power,
        Sensors,
        AttitudeAccel,
        AttitudeGyro,
        GPS,
        DateTime,
    };

    typedef ImsMessageBase<IMS_Sensors>       ImsMessageSensors;
    typedef ImsMessageBase<IMS_Status>        ImsMessageStatus;
    typedef ImsMessageBase<IMS_Power>         ImsMessagePower;
    typedef ImsMessageBase<IMS_AttitudeAccel> ImsMessageAccel;
    typedef ImsMessageBase<IMS_AttitudeGyro>  ImsMessageGyro;
    typedef ImsMessageBase<IMS_GPS>           ImsMessageGPS;
    typedef ImsMessageBase<IMS_DateTime>      ImsMessageDateTime;
}
