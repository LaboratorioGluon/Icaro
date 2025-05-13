#pragma once

#include <memory>

#include "data/systemstatus.h"

#include "network/wifiraw.h"
#include "interboards/i2cslave.h"

#include "data/i2cslavethreadstatus.h"
#include "data/externalstatus.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t> systemStatus;

    // Devices
    std::shared_ptr<Network::WiFiRaw>      wifiraw;
    std::shared_ptr<InterBoards::I2CSlave> i2cSlave;

    // Output info
    std::shared_ptr<Data::i2cSlaveThreadStatus_t> threadStatus;
    std::shared_ptr<Data::ExternalStatus_t>       externalStatus;
} i2cSlaveThreadArg_t;

void i2cSlaveThreadFunc (void* arg);
