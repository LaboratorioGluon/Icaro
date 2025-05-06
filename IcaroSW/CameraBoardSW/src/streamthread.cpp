#include "streamthread.h"

#include "config.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <string.h>

namespace
{
const char* MODULE_TAG = "TH_STREAM";

bool sendImage(std::shared_ptr<Device::ICamera>& camera, std::unique_ptr<Network::Link::ILink>& link)
{
    static int frameCounter = 0;
    bool imageSent = false;

    // 2.1 Grab frame
    Device::frame_t* frame = camera->grabFrame();
    if (frame != nullptr)
    {
        frameCounter = frameCounter + 1;
        ESP_LOGI(MODULE_TAG, "Grabbed frame: %d", frameCounter);
        ESP_LOGD(MODULE_TAG, "Frame size: %d Bytes", frame->len);
    
        // 2.2 Transform frame
    
        // 2.3 Send frame
        int err = link->write((char*)frame->buf, frame->len);
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

        imageSent = true;
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Wrong frame pointer: %d", frameCounter);
        imageSent = false;
    }
    return imageSent;
}

}

void streamThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<streamThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>& systemStatus = convertedArg->systemStatus;
    std::shared_ptr<Network::WiFiRaw>&           wifiraw      = convertedArg->wifiraw;
    std::shared_ptr<Device::ICamera>&            camera       = convertedArg->camera;
    std::shared_ptr<Data::streamThreadStatus_t>& status       = convertedArg->threadStatus;

    // 1.2 - Init thread data
    int delayCounter = 0;
    
    std::unique_ptr<Network::Link::ILink> cameraLink = wifiraw->create80211Link(Network::Link::RAW_LINK_ID::RAW_IMAGE);
    ESP_LOGI(MODULE_TAG, "Camera Link created");

    // 2 - Thread loop
    while (true)
    {
        if(systemStatus->streamingEnabled)
        {
            status->state = Data::ThreadState::RUNNING;

            // 2.1 Send image
            bool imageSent = sendImage(camera, cameraLink);

            // 2.2 Notify image sent through MAV
            if (imageSent)
            {
                // TODO: Send MAV message
            }

            // 2.N Sleep for watchdog
            delayCounter++;
            if (delayCounter > 200)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
                delayCounter = 0;
            }
        }
        else
        {
            status->state = Data::ThreadState::SLEEPING;

            // Sleep while thread is not enabled
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
        }

    }

    // 3 - Deinit thread
    status->state = Data::ThreadState::STOPPED;
}
