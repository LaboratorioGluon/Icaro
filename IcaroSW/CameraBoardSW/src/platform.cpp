#include "platform.h"

#include "device/icam.h"
#include "device/ifilesystem.h"

#include "device/camera/ov2640.h"
#include "device/filesystem/sdcard.h"

namespace Platform
{

std::shared_ptr<Device::ICamera> buildCamera()
{
    return std::make_shared<Device::Camera::OV2640>();
}

std::shared_ptr<Device::IFileSystem> buildFileSystem()
{
    return std::make_shared<Device::FileSystem::SDCard>();
}

std::shared_ptr<Network::WiFi> buildWiFi()
{
    return std::make_shared<Network::WiFi>();
}

std::shared_ptr<Network::WiFiRaw> buildWiFiRaw()
{   
    return std::make_shared<Network::WiFiRaw>();
}

}