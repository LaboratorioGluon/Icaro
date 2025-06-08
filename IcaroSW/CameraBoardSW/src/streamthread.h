#pragma once

#include <memory>

#include "data/systemstatus.h"
#include "data/externalstatus.h"

#include "device/icam.h"
#include "network/wifiraw.h"

#include "data/streamthreadstatus.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t>   systemStatus;
    std::shared_ptr<const Data::ExternalStatus_t> externalStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw> wifiraw;
    std::shared_ptr<Device::ICamera>  camera;

    // Output info
    std::shared_ptr<Data::streamThreadStatus_t> threadStatus;
} streamThreadArg_t;

void streamThreadFunc (void* arg);
