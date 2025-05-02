#pragma once

#include "config.h"

#if defined(WIFI_TYPE) && WIFI_TYPE == WIFIRAW

#include <pthread.h>
#include <memory>

#include "device/icam.h"
#include "network/wifiraw.h"

typedef struct
{
    std::shared_ptr<Device::ICamera> camera;
    std::shared_ptr<Network::WiFiRaw> wifiraw;
} witiThreadArg_t;

void wifiThreadFunc (void* arg);

#endif
