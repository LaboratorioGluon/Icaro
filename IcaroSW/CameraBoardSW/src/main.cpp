
#include <string>
#include <string.h>
#include <time.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "device/icam.h"
#include "device/ifilesystem.h"
#include "platform.h"

namespace
{
const char* MODULE_TAG = "MAIN";

Device::IFileSystem* fs = Platform::buildFileSystem();
Device::ICamera* camera = Platform::buildCamera();
}

bool initialize()
{
    bool initialized = true;

    esp_log_level_set(MODULE_TAG, ESP_LOG_VERBOSE);

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

    // Initialization
    if (!initialize())
    {
        ESP_LOGE(MODULE_TAG, "Initialization failed.");
        return;
    }

    // TODO: Implement and initialize RTC

    // Create initial file
    createInitFile();
    
    
    return;

    // Application main loop
    while(1)
    {
        // 1 -> Take picture
        // 2a -> Send picture
        // 2b -> Store picture
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
