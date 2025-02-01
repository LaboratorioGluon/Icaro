#include "ims_hal.h"

uint64_t getTimeUs()
{
#ifdef ESP_PLATFORM
    return esp32_getTimeUs();
#else
    return 0;
#endif
}