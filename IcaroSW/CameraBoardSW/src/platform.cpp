
#include "device/icam.h"
#include "device/ifilesystem.h"

#include "device/camera/ov2640.h"
#include "device/filesystem/sdcard.h"

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

}