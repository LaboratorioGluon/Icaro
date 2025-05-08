#pragma once

#include <stdint.h>

#include "systemstatus.h"
#include "states.h"

namespace Data
{

typedef struct
{
    ThreadState state;
    uint32_t    captureCount;
} storeThreadStatus_t;
    
}
