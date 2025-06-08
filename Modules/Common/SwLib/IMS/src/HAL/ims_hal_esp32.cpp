#include "ims_hal_esp32.h"

#include <esp_timer.h>

uint64_t esp32_getTimeUs()
{
    return esp_timer_get_time();
}
