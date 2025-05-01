
#include <string>
#include <string.h>
#include <time.h>
#include <memory>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

// #include "device/icam.h"
// #include "device/ifilesystem.h"
#include "platform.h"

// #define USE_WIFI
#define USE_WIFIRAW
#if defined(USE_WIFI)
#include "network/wifi.h"
#elif defined(USE_WIFIRAW)
#include "network/wifiraw.h"
#endif

namespace
{
const char* MODULE_TAG = "MAIN";

Device::IFileSystem* fs = Platform::buildFileSystem();
Device::ICamera* camera = Platform::buildCamera();
#if defined(USE_WIFI)
Network::WiFi* wifi = Platform::buildWiFi();
#include "network/wifi.h"
#elif defined(USE_WIFIRAW)
Network::WiFiRaw* wifi = Platform::buildWiFiRaw();
#endif

}

bool initialize()
{
    bool initialized = true;

    esp_log_level_set(MODULE_TAG, ESP_LOG_MAX);

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

    ESP_LOGI(MODULE_TAG, "Initializing wifi.");
    if (!wifi->initialize())
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize wifi.");
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

void try_connect()
{
#if defined(USE_WIFI)
    std::unique_ptr<Network::Link::ILink> udp_link {wifi->createUDPLink("192.168.3.5", 10000)};
    if (udp_link->connect())
    {
        ESP_LOGI(MODULE_TAG, "UDP Link connected.");
        
        std::string buffer {"Hello from udp connection!"};
        udp_link->write(buffer.c_str(), buffer.length());
    }
    
    std::unique_ptr<Network::Link::ILink> tcp_link = wifi->createTCPLink("192.168.3.5", 10001);
    if (tcp_link->connect())
    {
        ESP_LOGI(MODULE_TAG, "TCP Link connected.");
        
        std::string buffer {"Hello from tcp connection!"};
        tcp_link->write(buffer.c_str(), buffer.length());
    }
#elif defined(USE_WIFIRAW)
    std::unique_ptr<Network::Link::ILink> raw_link = wifi->create80211Link();
    ESP_LOGI(MODULE_TAG, "RAW Link created.");
    
    std::string buffer {"Hello from raw connection!"};
    int count = 0;
    
    while(1)
    {
        raw_link->write(buffer.c_str(), buffer.length());
        count++;
        if (count > 200)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            count = 0;
        }
    }
#endif
}

extern "C"
void app_main()
{
    ESP_LOGI(MODULE_TAG, "Hello Icaro.");

    // Initialization
    if (initialize())
    {
        ESP_LOGI(MODULE_TAG, "System initialized.");
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Initialization failed.");
        return;
    }

    // try_connect();

    // TODO: Implement and initialize RTC

    // Create initial file
    createInitFile();
    
    // Application data
    int count = 0;
    char imagefile[20];
    std::unique_ptr<Network::Link::ILink> raw_link = wifi->create80211Link();
    std::string buffer {"Hello from raw connection!"};

    // Application main loop
    ESP_LOGI(MODULE_TAG, "Starting application.");
    while(1)
    {
        count++;
        if (count % 200 == 0)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        sprintf(imagefile, "/%08d.jpg", count);

        // 1 -> Take picture
        ESP_LOGD(MODULE_TAG, "Image taken: %s", imagefile);
        Device::frame_t* frame = camera->takePicture();

        // 2a -> Send picture
        /* TODO: Send image */
        ESP_LOGD(MODULE_TAG, "Sending data: %s", imagefile);
        raw_link->write(buffer.c_str(), buffer.length());
        ESP_LOGD(MODULE_TAG, "Data sent: %s", imagefile);

        // 2b -> Store picture
        if(fs->write(imagefile, frame->buf, frame->len))
        {
            ESP_LOGD(MODULE_TAG, "Image stored: %s", imagefile);
        }
        else
        {
            ESP_LOGE(MODULE_TAG, "Failed to store image: %s", imagefile);
        }
    
        // 3 -> Free picture buffer
        camera->freeBuffer(frame);

        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
