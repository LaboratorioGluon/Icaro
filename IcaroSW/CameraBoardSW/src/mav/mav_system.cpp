#include "mav_system.h"

#include <minimal/mavlink_msg_heartbeat.h>
namespace
{

int16_t floatToMilliInt16(const float orig)
{
    constexpr int e3 = 1000;
    return static_cast<int16_t>(orig * e3);
}

int16_t floatToCentiInt16(const float orig)
{
    constexpr int e2 = 100;
    return static_cast<int16_t>(orig * e2);

}

int32_t doubleToDegE7Int32(const double orig)
{
    constexpr int e7 = 10000000;
    return static_cast<int32_t>(orig * e7);
}

int32_t doubleToMMInt32(const double orig)
{
    constexpr int e3 = 1000;
    return static_cast<int32_t>(orig * e3);
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

bool MAVSystem::sendSystemTime(uint64_t timestamp_us, uint32_t boottime_ms)
{
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_system_time_pack(
        systemID, componentID, 
        &mavMessage, 
        timestamp_us, 
        boottime_ms
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
    size_t message_length = mavlink_msg_raw_imu_pack(
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
        0,
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

bool MAVSystem::sendBattery(bool status3v3, bool status5v0, float batteryRemaining)
{
    const uint16_t voltages[10] = {UINT16_MAX};
    uint16_t voltages_ext[4] = {0};
    
    // Not official use
    voltages_ext[0] = (status3v3) ? 1.0 : 0.0;
    voltages_ext[1] = (status5v0) ? 1.0 : 0.0;

    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_battery_status_pack(
        systemID, componentID, 
        &mavMessage, 
        0, 
        MAV_BATTERY_FUNCTION_ALL,
        MAV_BATTERY_TYPE_UNKNOWN,
        INT16_MAX,
        voltages,
        -1,
        -1,
        -1,
        static_cast<int8_t>(batteryRemaining),
        0,
        MAV_BATTERY_CHARGE_STATE_UNDEFINED,
        voltages_ext,
        MAV_BATTERY_MODE_UNKNOWN,
        0
    );

    bool ok = (send(mavMessage) == message_length);
    return ok;
}

bool MAVSystem::sendNamedFloat(uint64_t timestamp, const char* name, float value)
{
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_named_value_float_pack(
        systemID, componentID, 
        &mavMessage, 
        timestamp,
        name, 
        value
    );
    bool ok = (send(mavMessage) == message_length);
    return ok;
}

}