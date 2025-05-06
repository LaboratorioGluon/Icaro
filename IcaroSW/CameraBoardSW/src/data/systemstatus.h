#pragma once

#include "states.h"

namespace Data
{

typedef struct
{
    AppState state;
    bool streamingEnabled;
    bool capturingEnabled;
} systemStatus_t;

}
