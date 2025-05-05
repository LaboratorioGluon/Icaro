#pragma once

#include "config.h"

#if defined(WIFI_TYPE) && WIFI_TYPE == WIFIRAW

#include <memory>

#include "network/wifiraw.h"

typedef struct
{
    std::shared_ptr<Network::WiFiRaw> wifiraw;
} mavStatusThreadArg_t;

void mavStatusThreadFunc (void* arg);

#endif
