#include "platform.h"

#include "device/icam.h"
#include "device/ifilesystem.h"

#include "device/camera/ov2640.h"
#include "device/filesystem/sdcard.h"

#include <driver/i2c.h>

namespace Platform
{

std::shared_ptr<Device::ICamera> buildCamera()
{
    // Uses I2C_NUM_1 for configuration of camera
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

std::shared_ptr<InterBoards::I2CSlave> buildI2CSlave()
{
    constexpr i2c_port_t i2cSlave  = I2C_NUM_0;
    constexpr uint16_t   slaveAddr = 0x90;
    #ifndef CAM32
    constexpr gpio_num_t sdaPin    = GPIO_NUM_47;
    constexpr gpio_num_t sclPin    = GPIO_NUM_21;
    #else
    constexpr gpio_num_t sdaPin    = GPIO_NUM_12;
    constexpr gpio_num_t sclPin    = GPIO_NUM_13;

    #endif
    constexpr uint32_t   clkSpeed  = 100000;
    return std::make_shared<InterBoards::I2CSlave>(i2cSlave, slaveAddr, sdaPin, sclPin, clkSpeed);
}

}