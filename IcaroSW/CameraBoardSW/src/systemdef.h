#pragma once

#include <common/mavlink.h>

// System
constexpr uint8_t SYSTEM_ID = 0x2E;


// Components
constexpr uint8_t COMP_ID_STORECAMERA = MAV_COMP_ID_CAMERA;
constexpr uint8_t COMP_ID_STREAMCAMERA = MAV_COMP_ID_CAMERA2;

constexpr uint8_t COMP_ID_CAMERABOARD        = MAV_COMP_ID_ONBOARD_COMPUTER;
constexpr uint8_t COMP_ID_SUPPLYBOARD        = MAV_COMP_ID_ONBOARD_COMPUTER2;
constexpr uint8_t COMP_ID_SENSORBOARD        = MAV_COMP_ID_ONBOARD_COMPUTER3;
constexpr uint8_t COMP_ID_COMMUNICATIONBOARD = MAV_COMP_ID_ONBOARD_COMPUTER4;

constexpr uint8_t COMP_ID_BATTERY = MAV_COMP_ID_BATTERY;
constexpr uint8_t COMP_ID_GPS     = MAV_COMP_ID_GPS;
constexpr uint8_t COMP_ID_IMU     = MAV_COMP_ID_IMU;

constexpr uint8_t COMP_ID_PARACHUTE = MAV_COMP_ID_PARACHUTE;