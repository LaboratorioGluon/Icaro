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
};

}
