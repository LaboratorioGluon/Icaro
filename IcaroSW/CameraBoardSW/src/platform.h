#pragma once 

#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/wifi.h"

namespace Platform
{

Device::ICamera* buildCamera();

Device::IFileSystem* buildFileSystem();

Network::WiFi* buildWiFi();

}