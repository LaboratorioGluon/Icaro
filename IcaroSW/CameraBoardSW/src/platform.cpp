
#include "device/icam.h"
#include "device/ifilesystem.h"

#include "device/camera/ov2640.h"
#include "device/filesystem/sdcard.h"

#include "network/wifi.h"
#include "network/wifiraw.h"
#include "network/link/ilink.h"

namespace 
{
static Network::WiFi* wifi = nullptr;
}

namespace Platform
{

Device::ICamera* buildCamera()
{
    return new Device::Camera::OV2640();
}

Device::IFileSystem* buildFileSystem()
{
    return new Device::FileSystem::SDCard();
}

Network::WiFi* buildWiFi()
{
    return new Network::WiFi();
}
