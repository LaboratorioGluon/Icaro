#include <string>
#include <string.h>
#include <time.h>
#include <memory>

// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>

#include <esp_log.h>

#include "config.h"

// Hardware
#include "platform.h"

// Threads
#include "storethread.h"

#if defined(WIFI_TYPE)
#if WIFI_TYPE == WIFI
#include "network/wifi.h"
#include "wifithread.h"
#elif WIFI_TYPE == WIFIRAW
#include "network/wifiraw.h"
#include "wifirawthread.h"
#else
#error "Wifi type not valid"
#endif
#else
#error "No wifi type defined"
#endif

namespace
{
const char* MODULE_TAG = "MAIN";

std::shared_ptr<Device::IFileSystem> fs = Platform::buildFileSystem();
std::shared_ptr<Device::ICamera> camera = Platform::buildCamera();

#if defined(WIFI_TYPE)
#if WIFI_TYPE == WIFI
std::shared_ptr<Network::WiFi> wifi = Platform::buildWiFi();
#elif WIFI_TYPE == WIFIRAW
std::shared_ptr<Network::WiFiRaw> wifi = Platform::buildWiFiRaw();
#else
#error "Wifi type not valid"
#endif
#else
#error "No wifi type defined"
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
#if defined(WIFI_TYPE)
#if WIFI_TYPE == WIFI
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
#elif WIFI_TYPE == WIFIRAW
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
    
    // Create threads
    witiThreadArg_t wifiArgs {
        .camera = camera,
        .wifiraw = wifi
    };
    
    storeThreadArg_t storeArgs {
        .camera = camera,
        .fs = fs
    };

    xTaskCreatePinnedToCore(
        wifiThreadFunc, // Función de la tarea
        "WifiTask",     // Nombre de la tarea
        4096,           // Tamaño de la pila
        &wifiArgs,      // Parámetros de la tarea
        1,              // Prioridad de la tarea
        NULL,           // Handler de la tarea
        0               // Núcleo al que se asigna la tarea (0 o 1)
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
    
    while(1)
    {
        vTaskDelay(INT_MAX);
    }
}

