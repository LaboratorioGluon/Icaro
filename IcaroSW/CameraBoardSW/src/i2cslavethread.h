#pragma once

#include <memory>

#include "data/i2cslavethreadstatus.h"
#include "interboards/i2cslave.h"

typedef struct
{
    // Input info
    std::shared_ptr<const Data::systemStatus_t> systemStatus;

    // Devices
    std::shared_ptr<InterBoards::I2CSlave>      i2cSlave;

    // Output info
    std::shared_ptr<Data::i2cSlaveThreadStatus_t> threadStatus;
} i2cSlaveThreadArg_t;

void i2cSlaveThreadFunc (void* arg);
