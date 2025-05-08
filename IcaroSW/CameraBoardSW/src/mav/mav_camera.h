#pragma once

#include "mav_component.h"

namespace MAVLink
{

class MAVCamera : MAVComponent
{
private:
    const uint8_t systemID;
    const uint8_t componentID;

public:
    MAVCamera(uint8_t systemID,
              uint8_t componentID,
              std::unique_ptr<Network::Link::ILink> link);
    virtual ~MAVCamera() = default;

    bool notifyCapture(int32_t index, int8_t result, const char* filename, 
                       int32_t lat, int32_t lon, int32_t alt);
};

}