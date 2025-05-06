#pragma once

#include <memory>

#include "data/storethreadstatus.h"
#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/wifiraw.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t> systemStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw>      wifiraw;
    std::shared_ptr<Device::ICamera>       camera;
    std::shared_ptr<Device::IFileSystem>   fs;

    // Output info
    std::shared_ptr<Data::storeThreadStatus_t> threadStatus;
} storeThreadArg_t;

void storeThreadFunc (void* arg);
