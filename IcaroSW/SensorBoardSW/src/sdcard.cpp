
#include "sdcard.h"
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <unistd.h>
#include <stdarg.h>
#include <dirent.h>
#include <esp_timer.h>


SDCard::SDCardConfig sdCardConfig = {
    .clk = GPIO_NUM_14,
    .cmd = GPIO_NUM_15,
    .data0 = GPIO_NUM_2,
    .data1 = GPIO_NUM_4,
    .data2 = GPIO_NUM_12,
    .data3 = GPIO_NUM_13
};

SDCard sdCard(sdCardConfig);

SDCard::SDCard(SDCardConfig config)
{
    sdConfig = config;
}

esp_err_t SDCard::init()
{
    esp_err_t ret;
    ESP_LOGI("SDCARD", "Initializing SD Card");
    
    // Initialize SD card here
    // Use sdConfig to configure the pins
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_cd = GPIO_NUM_39; // Card detect pin
    slot_config.width = 4; // 4-bit bus width

    slot_config.clk = sdConfig.clk;
    slot_config.cmd = sdConfig.cmd;
    slot_config.d0 = sdConfig.data0;
    slot_config.d1 = sdConfig.data1;
    slot_config.d2 = sdConfig.data2;
    slot_config.d3 = sdConfig.data3;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE("SDCARD", "Failed to mount filesystem. Error: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGE("SDCARD", "SD Card mounted successfully");

    sdmmc_card_print_info(stdout, card);


    // Create files for logging data and events with the next available number
    ESP_LOGI("SDCARD", "Creating data and event files");
    dataFile = NULL;
    eventFile = NULL;   

    // Find the next available dataN.bin file
    int fileNumber = 0;
    char dataFileName[32];
    
    while (true) {
        snprintf(dataFileName, sizeof(dataFileName), "/sdcard/data%d.bin", fileNumber);
        dataFile = fopen(dataFileName, "r");
        if (dataFile == NULL) {
            // File does not exist, we can use this name
            break;
        }
        fclose(dataFile);
        fileNumber++;
    }

    dataFile = fopen(dataFileName, "wb");
    snprintf(dataFileName, sizeof(dataFileName), "/sdcard/event%d.txt", fileNumber);
    eventFile = fopen(dataFileName, "w");

    

    return ret;
}

esp_err_t SDCard::logData(SensorData *data)
{
    esp_err_t ret= ESP_OK;
    ESP_LOGI("SDCARD", "Logging data");
    
    // Log data to SD card here
    if (dataFile == NULL) {
        ESP_LOGE("SDCARD", "Data file not opened");
        return ESP_FAIL;
    }

    fwrite(data, sizeof(SensorData), 1, dataFile);
    
    fflush(dataFile);
    fsync(fileno(dataFile));

    return ret;
}

esp_err_t SDCard::logEvent(const char *fmt, ...)
{
    esp_err_t ret = ESP_OK;
    
    if (eventFile == NULL) {
        ESP_LOGE("SDCARD", "Event file not opened");
        return ESP_FAIL;
    }

    va_list args;
    va_start(args, fmt);
    ESP_LOGI("SDCARD", "Logging event");
    fprintf(eventFile, "%lld:", esp_timer_get_time()); // Log timestamp in milliseconds
    vfprintf(eventFile, fmt, args);
    va_end(args);
    ret = fflush(eventFile);
    fsync(fileno(eventFile));


    return ret;
}