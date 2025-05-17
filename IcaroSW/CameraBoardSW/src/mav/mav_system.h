#pragma once

#include "mav_component.h"

namespace MAVLink
{

class MAVSystem : MAVComponent
{
private:
    const uint8_t systemID;
    const uint8_t componentID;
    const uint8_t systemType;

public:
    MAVSystem(uint8_t systemID,
              uint8_t componentID,
              uint8_t systemType,
              std::unique_ptr<Network::Link::ILink> link);
    virtual ~MAVSystem() = default;

    bool sendHeartBeat(uint8_t  system_status = 0,
                       uint32_t custom_mode   = 0,
                       uint8_t  base_mode     = 0);

    bool sendScaledIMU(uint64_t timestamp,
                       float accelX,    float accelY,    float accelZ,
                       float gyroX,     float gyroY,     float gyroZ,
                       float magX=0.0f, float magY=0.0f, float magZ=0.0f,
                       float temp=0.0f);

    bool sendGPS(uint64_t timestamp, double lat, double lon, double alt);
};

}
