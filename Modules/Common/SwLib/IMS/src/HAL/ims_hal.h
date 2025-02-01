#ifndef IMS_HAL__
#define IMS_HAL__

#include <stdint.h>

#ifdef ESP_PLATFORM
    #include "ims_hal_esp32.h"
#else
#endif

uint64_t getTimeUs();

#endif