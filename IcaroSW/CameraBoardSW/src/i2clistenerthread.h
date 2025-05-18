#pragma once

#include <memory>

#include "data/systemstatus.h"

#include "network/wifiraw.h"
#include "interboards/i2cslave.h"

#include "data/i2clistenerthreadstatus.h"
#include "data/externalstatus.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t> systemStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw>      wifiraw;
    std::shared_ptr<InterBoards::I2CSlave> i2cSlave;

    // Output info
    std::shared_ptr<Data::i2cListenerThreadStatus_t> threadStatus;
    std::shared_ptr<Data::ExternalStatus_t>       externalStatus;
} i2cListenerThreadArg_t;

void i2cListenerThreadFunc (void* arg);
