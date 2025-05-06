#include "mav_system.h"

#include "../systemdef.h"

namespace MAVLink
{

MAVSystem::MAVSystem(uint8_t systemID,
                     uint8_t componentID,
                     uint8_t systemType,
                     std::unique_ptr<Network::Link::ILink> link) : 
    MAVComponent(std::move(link)), systemID(systemID), componentID(componentID), 
    systemType(systemType)
{}

void MAVSystem::sendHeartBeat(uint8_t system_status, uint32_t custom_mode, uint8_t base_mode)
{
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_heartbeat_pack(
        systemID, componentID, 
        &mavMessage, 
        systemType, 
        MAV_AUTOPILOT_INVALID,
        base_mode,
        custom_mode,
        system_status
    );
    send(mavMessage);
}

}
