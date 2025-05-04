#include "mavstatusthread.h"

#include "config.h"

#if defined(WIFI_TYPE) && WIFI_TYPE == WIFIRAW

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <common/mavlink.h>

#include <esp_log.h>

namespace
{
const char* MODULE_TAG = "TH_MAV";

constexpr size_t MAX_MESSAGE_LENGTH = 256;
uint8_t mavBuffer [MAX_MESSAGE_LENGTH];
void sendHeartBeat(std::unique_ptr<Network::Link::ILink>& link)
{
    constexpr int SYSTEM_ID = 0xA5;
    constexpr int COMPONENT_ID = 0x5A;
    
    // Build MAV message
    mavlink_message_t mavMessage;
    size_t message_length = mavlink_msg_heartbeat_pack(
        SYSTEM_ID, COMPONENT_ID, 
        &mavMessage, 
        MAV_TYPE_FREE_BALLOON, 
        MAV_AUTOPILOT_INVALID,
        (uint8_t) 0x00,
        (uint32_t) 0xA5A5A5A5,
        MAV_STATE_ACTIVE
    );

    // Build message buffer to send
    size_t mavBuffer_length = mavlink_msg_to_send_buffer(mavBuffer, &mavMessage);

    int err = link->write(reinterpret_cast<char*>(mavBuffer), mavBuffer_length);
    if (err < 0)
    {
        ESP_LOGE(MODULE_TAG, "Failed to send MAVLink Heartbeat");
    }
    else if (err < message_length)
    {
        ESP_LOGE(MODULE_TAG, "Partially failed to send MAVLink Heartbeat");
    }
}

}

void mavStatusThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<mavStatusThreadArg_t*>(arg);
    std::shared_ptr<Network::WiFiRaw>& wifiraw = convertedArg->wifiraw;

    // 1.2 - Init thread data
    TickType_t lastWakeUpTime = 0;
    


    std::unique_ptr<Network::Link::ILink> mavLink = wifiraw->create80211Link(Network::Link::RAW_LINK_ID::MAV_STATUS);
    ESP_LOGI(MODULE_TAG, "MAV Link created");
    
    // 2 - Thread loop
    while (true)
    {
        // 2.1 Send MAV link heartbeat
        sendHeartBeat(mavLink);

        // 2.N Sleep
        xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
    }

    // 3 - Deinit thread

}

#endif
