#include "storethread.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
#include "data/states.h"
#include "mav/mav_camera.h"

namespace
{
const char*           MODULE_TAG       = "TH_STORE";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_NONE;
}

void storeThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<storeThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>&   systemStatus   = convertedArg->systemStatus;
    std::shared_ptr<const Data::ExternalStatus_t>& externalStatus = convertedArg->externalStatus;
    std::shared_ptr<Network::WiFiRaw>&             wifi           = convertedArg->wifiraw;
    std::shared_ptr<Device::ICamera>&              camera         = convertedArg->camera;
    std::shared_ptr<Device::IFileSystem>&          fs             = convertedArg->fs;
    std::shared_ptr<Data::storeThreadStatus_t>&    status         = convertedArg->threadStatus;

    // 1.2 - Init thread data
    int frameCounter = 0;
    char imagefile[20];

    MAVLink::MAVCamera mavCamera (SYSTEM_ID,
                                COMP_ID_STORECAMERA,
                                wifi->create80211Link(Network::Link::RAW_LINK_ID::MAV_DATA));

    // 2 - Thread loop
    while (true)
    {
        ESP_LOGV(MODULE_TAG, "Cycle start");
        if(systemStatus->capturingEnabled)
        {
            status->state = Data::ThreadState::RUNNING;

            // 2.1 Grab frame
            Device::frame_t* frame = camera->grabFrame();
            frameCounter = frameCounter + 1;
            ESP_LOGD(MODULE_TAG, "Grabbed frame: %d", frameCounter);
            
            // 2.2 Transform frame
            
            // 2.3 Store frame
            sprintf(imagefile, "/%08d.jpg", frameCounter);
            bool imageStored = fs->write(imagefile, frame->buf, frame->len);
            
            // 2.4 Free frame
            camera->freeFrame(frame);
            
            if (imageStored)
            {
                ESP_LOGD(MODULE_TAG, "Image stored: %s", imagefile);
            }
            else
            {
                ESP_LOGE(MODULE_TAG, "Failed to store image: %s", imagefile);
            }
            
            // 2.5 Notify captured image
            mavCamera.notifyCapture(frameCounter, imageStored, imagefile, 
                                    externalStatus->gps.latitude, 
                                    externalStatus->gps.longitude, 
                                    externalStatus->gps.altitude);

            // Limit cycles per minute (max)
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(50));
        }
        else
        {
            ESP_LOGV(MODULE_TAG, "Sleeping");
            status->state = Data::ThreadState::SLEEPING;

            // Sleep while thread is not enabled
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
        }
    }
    
    // 3 - Deinit thread
    ESP_LOGE(MODULE_TAG, "Fatal error: Thread stopped");
    status->state = Data::ThreadState::STOPPED;
}
