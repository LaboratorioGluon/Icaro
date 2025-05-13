#pragma once

#include <stdint.h>

#include "systemstatus.h"
#include "states.h"

namespace Data
{

typedef struct
{
    ThreadState state;
} i2cSlaveThreadStatus_t;
    
}
