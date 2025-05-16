#pragma once 

#include <memory>

#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/wifi.h"
#include "network/wifiraw.h"
#include "interboards/i2cslave.h"

namespace Platform
{

std::shared_ptr<Device::ICamera> buildCamera();

std::shared_ptr<Device::IFileSystem> buildFileSystem();

std::shared_ptr<Network::WiFi> buildWiFi();

std::shared_ptr<Network::WiFiRaw> buildWiFiRaw();

std::shared_ptr<InterBoards::I2CSlave> buildI2CSlave();

}