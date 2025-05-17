#include "mav_system.h"

#include <minimal/mavlink_msg_heartbeat.h>

namespace
{

int16_t floatToMilliInt16(const float orig)
{
    constexpr int e3 = 1000;
    int units = static_cast<int> (orig);
    int decimals = static_cast<int> ((orig-units) * e3);
    int16_t converted = (units * e3) + decimals;
    return converted;
}

int16_t floatToCentiInt16(const float orig)
{
    constexpr int e2 = 100;
    int units = static_cast<int> (orig);
    int decimals = static_cast<int> ((orig-units) * e2);
    int16_t converted = (units * e2) + decimals;
    return converted;
}

int32_t doubleToDegE7Int32(const double orig)
{
    constexpr int e7 = 10000000;
    int units = static_cast<int> (orig);
    int decimals = static_cast<int> ((orig-units) * e7);
    int16_t converted = (units * e7) + decimals;
    return converted;

}

int32_t doubleToMMInt32(const double orig)
{
    constexpr int e3 = 1000;
    int units = static_cast<int> (orig);
    int decimals = static_cast<int> ((orig-units) * e3);
    int16_t converted = (units * e3) + decimals;
    return converted;
}

}

namespace MAVLink
{

MAVSystem::MAVSystem(uint8_t systemID,
                     uint8_t componentID,
                     uint8_t systemType,
                     std::unique_ptr<Network::Link::ILink> link) : 
    MAVComponent(std::move(link)), systemID(systemID), componentID(componentID), 
    systemType(systemType)
{}

bool MAVSystem::sendHeartBeat(uint8_t system_status, uint32_t custom_mode, uint8_t base_mode)
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

    bool ok = (send(mavMessage) == message_length);
    return ok;
}

bool MAVSystem::sendScaledIMU(uint64_t timestamp,
                              float accelX, float accelY, float accelZ,
                              float gyroX,  float gyroY,  float gyroZ,
                              float magX,   float magY,   float magZ,
                              float temp)
{
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_scaled_imu_pack(
        systemID, componentID, 
        &mavMessage, 
        timestamp, 
        floatToMilliInt16(accelX),
        floatToMilliInt16(accelY),
        floatToMilliInt16(accelZ),
        floatToMilliInt16(gyroX),
        floatToMilliInt16(gyroY),
        floatToMilliInt16(gyroZ),
        floatToMilliInt16(magX),
        floatToMilliInt16(magY),
        floatToMilliInt16(magZ),
        floatToCentiInt16(temp)
    );

    bool ok = (send(mavMessage) == message_length);
    return ok;
}

bool MAVSystem::sendGPS(uint64_t timestamp, double lat, double lon, double alt)
{
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_global_position_int_pack(
        systemID, componentID, 
        &mavMessage, 
        timestamp, 
        doubleToDegE7Int32(lat),
        doubleToDegE7Int32(lon),
        doubleToMMInt32(alt),
        0,
        0,
        0,
        0,
        UINT16_MAX
    );

    bool ok = (send(mavMessage) == message_length);
    return ok;
}

}
