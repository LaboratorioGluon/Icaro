#pragma once

#include <memory>

#include "data/streamthreadstatus.h"
#include "device/icam.h"
#include "network/wifiraw.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t> systemStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw> wifiraw;
    std::shared_ptr<Device::ICamera>  camera;

    // Output info
    std::shared_ptr<Data::streamThreadStatus_t> threadStatus;
} streamThreadArg_t;

void streamThreadFunc (void* arg);
