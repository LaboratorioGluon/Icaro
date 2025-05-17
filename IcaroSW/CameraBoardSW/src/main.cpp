#include <string>
#include <string.h>
#include <time.h>
#include <memory>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
// #include "config.h"
#include "data/systemstatus.h"
#include "data/externalstatus.h"

// Hardware
#include "platform.h"
#include "interboards/i2cslave.h"
#include "network/wifiraw.h"

// Threads
#include "storethread.h"
#include "data/storethreadstatus.h"

#include "streamthread.h"
#include "data/streamthreadstatus.h"

#include "i2clistenerthread.h"
#include "data/i2clistenerthreadstatus.h"

// Main dependencies
#include "mav/mav_system.h"


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

std::shared_ptr<Data::i2cListenerThreadStatus_t> i2cListenerThreadStatus = std::make_shared<Data::i2cListenerThreadStatus_t>();
std::shared_ptr<const Data::i2cListenerThreadStatus_t> c_i2cListenerThreadStatus = std::const_pointer_cast<const Data::i2cListenerThreadStatus_t>(i2cListenerThreadStatus);

std::shared_ptr<Data::ExternalStatus_t> externalStatus = std::make_shared<Data::ExternalStatus_t>();
std::shared_ptr<const Data::ExternalStatus_t> c_externalStatus = std::const_pointer_cast<const Data::ExternalStatus_t>(externalStatus);

/* Devices */
std::shared_ptr<Device::IFileSystem> fs = Platform::buildFileSystem();
std::shared_ptr<Device::ICamera> camera = Platform::buildCamera();

std::shared_ptr<Network::WiFiRaw> wifi = Platform::buildWiFiRaw();
std::shared_ptr<InterBoards::I2CSlave> i2cSlave = Platform::buildI2CSlave();

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

    ESP_LOGI(MODULE_TAG, "Initializing i2c slave.");
    if (!i2cSlave->initialize())
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize i2c slave.");
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

void sendStatusMAV(std::shared_ptr<const Data::ExternalStatus_t> c_externalStatus, MAVLink::MAVSystem& mavSystem)
{
    auto now   = static_cast<uint64_t>(time(NULL));
    auto accel = c_externalStatus->accel;
    auto gyro  = c_externalStatus->gyro;
    auto gps   = c_externalStatus->gps;

    mavSystem.sendScaledIMU(now, accel.accelX, accel.accelY, accel.accelZ,
                                gyro.gyroX, gyro.gyroY, gyro.gyroZ);
    mavSystem.sendGPS(now, gps.latitude, gps.longitude, gps.altitude);
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

    MAVLink::MAVSystem mavSystem(SYSTEM_ID,
                            COMP_ID_CAMERABOARD,
                            MAV_TYPE_FREE_BALLOON,
                            wifi->create80211Link(Network::Link::RAW_LINK_ID::MAV_STATUS));

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
            .systemStatus   = c_systemStatus,
            .externalStatus = c_externalStatus,
            .wifiraw        = wifi,
            .camera         = camera,
            .threadStatus   = streamThreadStatus,
        };
        
        storeThreadArg_t storeArgs {
            .systemStatus   = c_systemStatus,
            .externalStatus = c_externalStatus,
            .wifiraw        = wifi,
            .camera         = camera,
            .fs             = fs,
            .threadStatus   = storeThreadStatus,
        };

        i2cListenerThreadArg_t i2cListenerArgs {
            .systemStatus   = c_systemStatus,
            .wifiraw        = wifi,
            .i2cSlave       = i2cSlave,
            .threadStatus   = i2cListenerThreadStatus,
            .externalStatus = externalStatus,
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

        xTaskCreatePinnedToCore(
            i2cListenerThreadFunc, // Función de la tarea
            "i2cListenerTask",     // Nombre de la tarea
            4096,                  // Tamaño de la pila
            &i2cListenerArgs,      // Parámetros de la tarea
            1,                     // Prioridad de la tarea
            NULL,                  // Handler de la tarea
            0                      // Núcleo al que se asigna la tarea (0 o 1)
        );
    }

    // Start
    systemStatus->state = Data::AppState::FULL_POWER;
    mavSystem.sendHeartBeat(MAV_STATE_BOOT, static_cast<uint32_t>(systemStatus->state));


    // State machine controller
    while(1)
    {
        int delayMS = 500;
        switch(systemStatus->state)
        {            
            case Data::AppState::FULL_POWER:
                mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));
                
                // Configure threads
                systemStatus->capturingEnabled = false;
                systemStatus->streamingEnabled = false;
                systemStatus->i2cListenEnabled = true;

                // Do actions
                sendStatusMAV(c_externalStatus, mavSystem);

                // Check transitions
                if (false) // TODO: Condition for transition
                {
                    systemStatus->state = Data::AppState::POWER_SAVE;
                }
            break;

            case Data::AppState::POWER_SAVE:
                // Send 2 hearbeats 
                for (int i=0; i<=2; i++)
                {
                    mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));
                }

                // Configure threads
                systemStatus->capturingEnabled = true;
                systemStatus->streamingEnabled = false;
                systemStatus->i2cListenEnabled = true;

                // Do actions
                delayMS = 1000;

                // Check transitions
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
                // Send 2 hearbeats 
                for (int i=0; i<=10; i++)
                {
                    mavSystem.sendHeartBeat(MAV_STATE_ACTIVE, static_cast<uint32_t>(systemStatus->state));
                }

                // Do actions
                delayMS = 5000;

                // Check transitions
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

                // Do actions

                // Check transitions
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
        xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(delayMS));
    }
}

