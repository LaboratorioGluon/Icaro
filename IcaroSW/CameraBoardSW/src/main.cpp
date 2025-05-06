#include <string>
#include <string.h>
#include <time.h>
#include <memory>

// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
// #include "config.h"
#include "data/systemstatus.h"

// Hardware
#include "platform.h"

// Threads
#include "storethread.h"
#include "data/storethreadstatus.h"

// Main dependencies
#include "mav/mav_system.h"

// #if defined(WIFI_TYPE)
// #if WIFI_TYPE == WIFI
// #include "network/wifi.h"
// #elif WIFI_TYPE == WIFIRAW
#include "network/wifiraw.h"
#include "streamthread.h"
#include "data/streamthreadstatus.h"
// #include "data/mavthreadstatus.h"
// #else
// #error "Wifi type not valid"
// #endif
// #else
// #error "No wifi type defined"
// #endif

namespace
{
const char* MODULE_TAG = "MAIN";


/* Threads status */
std::shared_ptr<Data::systemStatus_t> systemStatus = std::make_shared<Data::systemStatus_t>();
std::shared_ptr<const Data::systemStatus_t> c_systemStatus = std::const_pointer_cast<const Data::systemStatus_t>(systemStatus);

std::shared_ptr<Data::storeThreadStatus_t> storeThreadStatus = std::make_shared<Data::storeThreadStatus_t>();
std::shared_ptr<const Data::storeThreadStatus_t> c_storeThreadStatus = std::const_pointer_cast<const Data::storeThreadStatus_t>(storeThreadStatus);

std::shared_ptr<Data::streamThreadStatus_t> streamThreadStatus = std::make_shared<Data::streamThreadStatus_t>();
std::shared_ptr<const Data::streamThreadStatus_t> c_streamThreadStatus = std::const_pointer_cast<const Data::streamThreadStatus_t>(streamThreadStatus);

/* Devices */
std::shared_ptr<Device::IFileSystem> fs = Platform::buildFileSystem();
std::shared_ptr<Device::ICamera> camera = Platform::buildCamera();

// #if defined(WIFI_TYPE)
// #if WIFI_TYPE == WIFI
// std::shared_ptr<Network::WiFi> wifi = Platform::buildWiFi();
// #elif WIFI_TYPE == WIFIRAW
std::shared_ptr<Network::WiFiRaw> wifi = Platform::buildWiFiRaw();
// #else
// #error "Wifi type not valid"
// #endif
// #else
// #error "No wifi type defined"
// #endif
}

bool initialize()
{
    bool initialized = true;

    esp_log_level_set(MODULE_TAG, ESP_LOG_MAX);

    ESP_LOGI(MODULE_TAG, "Initializing wifi.");
    if (!wifi->initialize())
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize wifi.");
        initialized = false;
    }

    ESP_LOGI(MODULE_TAG, "Initializing file system.");
    if (!fs->initialize())
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize file system.");
        initialized = false;
    }
    
    ESP_LOGI(MODULE_TAG, "Initializing camera.");
    if (!camera->initialize())
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize camera.");
        initialized = false;
    }

    return initialized;
}

void createInitFile()
{
    const std::string file = "/boot.txt";

    #define DATE_SIZE    20
    char timeBuffer[DATE_SIZE];
    
    time_t currentTime = time(nullptr);
    strftime(timeBuffer, 20, "%d/%m/%Y %H:%M:%S", localtime(&currentTime));
    
    ESP_LOGI(MODULE_TAG, "Boot at %s", timeBuffer);

    #define BUFFER_SIZE    64
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "Last boot at %s\n", timeBuffer);

    bool writeOk = fs->append(file, (uint8_t*)buffer, strlen(buffer));
    if (writeOk)
    {
        ESP_LOGI(MODULE_TAG, "Logged boot time.");
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Error writting %s file.", file.c_str());
    }
}

extern "C"
void app_main()
{
    ESP_LOGI(MODULE_TAG, "Hello Icaro.");
    systemStatus->state = Data::AppState::BOOTING;

    // Initialization
    systemStatus->state = Data::AppState::INITIALIZING;
    if (initialize())
    {
        ESP_LOGI(MODULE_TAG, "System initialized.");
    }
    else
    {
        systemStatus->state = Data::AppState::TOTAL_FAILURE;
        ESP_LOGE(MODULE_TAG, "Initialization failed.");
        return;
    }

    
// #if defined(WIFI_TYPE)
// #if WIFI_TYPE == WIFI
//     MAVLink::MAVSystem system(SYSTEM_ID,
//                             COMP_ID_CAMERABOARD,
//                             MAV_TYPE_FREE_BALLOON,
//                             wifi->createUDPLink(UDP_IP_ADDRESS, UDP_IP_PORT));
// #elif WIFI_TYPE == WIFIRAW
    MAVLink::MAVSystem mavSystem(SYSTEM_ID,
                            COMP_ID_CAMERABOARD,
                            MAV_TYPE_FREE_BALLOON,
                            wifi->create80211Link(Network::Link::RAW_LINK_ID::MAV_STATUS));
// #else
// #error "Wifi type not valid"
// #endif
// #else
// #error "No wifi type defined"
// #endif

    // Send 5 hearbeats in booting sequence
    for (int i=0; i<=5; i++)
    {
        mavSystem.sendHeartBeat(MAV_STATE_BOOT, static_cast<uint32_t>(systemStatus->state));
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    // TODO: Implement and initialize RTC?

    // Create initial file
    createInitFile();
    
    // Create threads
    {

        streamThreadArg_t streamArgs {
            .systemStatus = c_systemStatus,
            .wifiraw = wifi,
            .camera = camera,
            .threadStatus = streamThreadStatus
        };
        
        storeThreadArg_t storeArgs {
            .systemStatus = c_systemStatus,
            .wifiraw = wifi,
            .camera = camera,
            .fs = fs,
            .threadStatus = storeThreadStatus,
        };
                
        xTaskCreatePinnedToCore(
            streamThreadFunc, // Función de la tarea
            "StreamTask",     // Nombre de la tarea
            4096,             // Tamaño de la pila
            &streamArgs,      // Parámetros de la tarea
            1,                // Prioridad de la tarea
            NULL,             // Handler de la tarea
            0                 // Núcleo al que se asigna la tarea (0 o 1)
        );
        
        xTaskCreatePinnedToCore(
            storeThreadFunc, // Función de la tarea
            "StoreTask",     // Nombre de la tarea
            4096,            // Tamaño de la pila
            &storeArgs,      // Parámetros de la tarea
            1,               // Prioridad de la tarea
            NULL,            // Handler de la tarea
            0                // Núcleo al que se asigna la tarea (0 o 1)
        );
    }

    // Start
    systemStatus->state = Data::AppState::FULL_POWER;
    mavSystem.sendHeartBeat(MAV_STATE_BOOT, static_cast<uint32_t>(systemStatus->state));


    // State machine controller
    while(1)
    {
        switch(systemStatus->state)
        {            
            case Data::AppState::FULL_POWER:
                mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));

                systemStatus->capturingEnabled = true;
                systemStatus->streamingEnabled = false;

                if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::POWER_SAVE;
                }
            break;

            case Data::AppState::POWER_SAVE:
                mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));
                if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::FULL_POWER;
                }
                else if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::BEACON;
                }
            break;

            case Data::AppState::BEACON:
                mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));
                if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::FULL_POWER;
                }
                else if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::POWER_SAVE;
                }
                break;

            case Data::AppState::RECOVERY:
            default:
                mavSystem.sendHeartBeat(MAV_STATE_CRITICAL, static_cast<uint32_t>(systemStatus->state));
                if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::FULL_POWER;
                }
                else if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::POWER_SAVE;
                }
                else 
                {
                    systemStatus->state = Data::AppState::BEACON;
                }
            break;
        }

        // Delay between transition checks
        static TickType_t lastWakeUpTime = 0;
        xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
    }
}

