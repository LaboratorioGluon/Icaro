#pragma once 

#include <memory>

#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/wifi.h"
#include "network/wifiraw.h"

namespace Platform
{

std::shared_ptr<Device::ICamera> buildCamera();

std::shared_ptr<Device::IFileSystem> buildFileSystem();

std::shared_ptr<Network::WiFi> buildWiFi();

std::shared_ptr<Network::WiFiRaw> buildWiFiRaw();

}