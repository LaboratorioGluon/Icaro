#pragma once

#include <memory>

#include "data/systemstatus.h"
#include "data/externalstatus.h"

#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/wifiraw.h"

#include "data/storethreadstatus.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t>   systemStatus;
    std::shared_ptr<const Data::ExternalStatus_t> externalStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw>      wifiraw;
    std::shared_ptr<Device::ICamera>       camera;
    std::shared_ptr<Device::IFileSystem>   fs;

    // Output info
    std::shared_ptr<Data::storeThreadStatus_t> threadStatus;
} storeThreadArg_t;

void storeThreadFunc (void* arg);
