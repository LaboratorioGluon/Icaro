#include "streamthread.h"

#include "config.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
#include "data/states.h"
#include "mav/mav_camera.h"

namespace
{
const char* EMPTY_STRING = "";
const char* MODULE_TAG = "TH_STREAM";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_NONE;

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

        imageSent = (err == frame->len);
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
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<streamThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>&   systemStatus   = convertedArg->systemStatus;
    std::shared_ptr<const Data::ExternalStatus_t>  externalStatus = convertedArg->externalStatus;
    std::shared_ptr<Network::WiFiRaw>&             wifi           = convertedArg->wifiraw;
    std::shared_ptr<Device::ICamera>&              camera         = convertedArg->camera;
    std::shared_ptr<Data::streamThreadStatus_t>&   status         = convertedArg->threadStatus;

    // 1.2 - Init thread data
    std::unique_ptr<Network::Link::ILink> cameraLink = wifi->create80211Link(Network::Link::RAW_LINK_ID::RAW_IMAGE);
    ESP_LOGI(MODULE_TAG, "Camera streaming link created");

    MAVLink::MAVCamera mavCamera (SYSTEM_ID,
        COMP_ID_STREAMCAMERA,
        wifi->create80211Link(Network::Link::RAW_LINK_ID::MAV_DATA));

    // 2 - Thread loop
    while (true)
    {
        if(systemStatus->streamingEnabled)
        {
            status->state = Data::ThreadState::RUNNING;

            // 2.1 Send image
            bool imageSent = sendImage(camera, cameraLink);

            // 2.2 Notify image sent through MAV
            static int frameCounter = 0;
            mavCamera.notifyCapture(frameCounter++, imageSent, EMPTY_STRING, 0,0,0);

            // Limit cycles per minute (max)
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(40));            
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
