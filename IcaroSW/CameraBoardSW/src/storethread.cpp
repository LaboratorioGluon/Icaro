#include "storethread.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

namespace
{
const char* MODULE_TAG = "TH_STORE";
}

void storeThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<storeThreadArg_t*>(arg);
    std::shared_ptr<Device::ICamera>& camera = convertedArg->camera;
    std::shared_ptr<Device::IFileSystem>& fs = convertedArg->fs;

    // 1.2 - Init thread data
    int delayCounter = 0;
    int frameCounter = 0;
    char imagefile[20];

    // 2 - Thread loop
    while (true)
    {
        // 2.1 Grab frame
        Device::frame_t* frame = camera->grabFrame();
        frameCounter = frameCounter + 1;
        ESP_LOGD(MODULE_TAG, "Grabbed frame: %d", frameCounter);

        // 2.2 Transform frame

        // 2.3 Store frame
        sprintf(imagefile, "/%08d.jpg", frameCounter);
        if(fs->write(imagefile, frame->buf, frame->len))
        {
            ESP_LOGD(MODULE_TAG, "Image stored: %s", imagefile);
        }
        else
        {
            ESP_LOGE(MODULE_TAG, "Failed to store image: %s", imagefile);
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