#include "wifirawthread.h"

#include "config.h"

#if defined(WIFI_TYPE) && WIFI_TYPE == WIFIRAW

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <string.h>

namespace
{
const char* MODULE_TAG = "TH_WIFI";
}

void wifiThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<witiThreadArg_t*>(arg);
    std::shared_ptr<Device::ICamera>& camera = convertedArg->camera;
    std::shared_ptr<Network::WiFiRaw>& wifiraw = convertedArg->wifiraw;

    // 1.2 - Init thread data
    int delayCounter = 0;
    int frameCounter = 0;
    
    std::unique_ptr<Network::Link::ILink> raw_link = wifiraw->create80211Link();
    ESP_LOGI(MODULE_TAG, "RAW Link created");
    
    // 2 - Thread loop
    while (true)
    {
        // 2.1 Grab frame
        Device::frame_t* frame = camera->grabFrame();
        frameCounter = frameCounter + 1;
        ESP_LOGI(MODULE_TAG, "Grabbed frame: %d", frameCounter);
        ESP_LOGD(MODULE_TAG, "Frame size: %d Bytes", frame->len);

        // 2.2 Transform frame

        // 2.3 Send frame
        int err = raw_link->write((char*)frame->buf, frame->len);
        if (err < 0)
        {
            ESP_LOGE(MODULE_TAG, "Failed to send frame: %d", frameCounter);
        }
        else if (err < frame->len)
        {
            ESP_LOGE(MODULE_TAG, "Partially failed to send frame: %d", frameCounter);
        }

        // 2.4 Free frame
        camera->freeFrame(frame);

        // 2.5 Sleep for watchdog
        delayCounter++;
        if (delayCounter > 200)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            delayCounter = 0;
        }
    }

    // 3 - Deinit thread

}

#endif
