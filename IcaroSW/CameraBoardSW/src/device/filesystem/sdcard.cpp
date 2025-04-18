#include "sdcard.h"

#include <esp_log.h>
#include <esp_vfs_fat.h>

#include <sdmmc_cmd.h>
#include <driver/sdmmc_host.h>

namespace
{
const char* MODULE_TAG = "SDCARD";

const char* MOUNT_POINT = "/sdcard";

// Bus definition
constexpr int SD_BUS_WIDTH = 1; 
constexpr gpio_num_t SD_PIN_CLK = GPIO_NUM_39;
constexpr gpio_num_t SD_PIN_CMD = GPIO_NUM_38;
constexpr gpio_num_t SD_PIN_D0 = GPIO_NUM_40;

}

namespace Device::FileSystem
{

SDCard::SDCard() : initialized(false), card(nullptr)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_INFO);    
}

bool SDCard::mount()
{
    bool mounted = false;

    // Host configuration
    ESP_LOGV(MODULE_TAG, "Configuring SD host.");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // Slot configuration
    ESP_LOGV(MODULE_TAG, "Configuring SD slot.");
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.width = SD_BUS_WIDTH;
    slot.clk   = SD_PIN_CLK;
    slot.cmd   = SD_PIN_CMD;
    slot.d0    = SD_PIN_D0;
    slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    // Mount configuration
    ESP_LOGV(MODULE_TAG, "Configuring SD mount.");
    esp_vfs_fat_sdmmc_mount_config_t mount = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    mount.allocation_unit_size = 16 * 1024;

    // Card configuration
    ESP_LOGI(MODULE_TAG, "Mounting SD card");
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot, &mount, &card);
    if (ret == ESP_OK) 
    {
        mounted = true;
    }
    else
    {
        mounted = false;
        ESP_LOGE(MODULE_TAG, "Failed to mount %s. (%s)", MOUNT_POINT, esp_err_to_name(ret));
    }

    return mounted;
}

bool SDCard::configure()
{
    /* Nothing to configure */
    return true;
}

bool SDCard::initialize()
{
    ESP_LOGD(MODULE_TAG, "Initializing SDCard.");
    
    if (mount())
    {
        initialized = true;
        ESP_LOGV(MODULE_TAG, "SDCard mounted");
        sdmmc_card_print_info(stdout, card);
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Failed mounting SDCard.");
    }

    return initialized;
}

bool SDCard::isAvailable() 
{
    return (card != nullptr) && initialized;
}

bool SDCard::write(const std::string& filepath, uint8_t* data, size_t data_len)
{
    ESP_LOGD(MODULE_TAG, "Writing file to SDCard.");
    bool written = false;

    std::string sdpath = MOUNT_POINT + filepath; 

    ESP_LOGD(MODULE_TAG, "Opening file %s.", sdpath.c_str());
    FILE *file = fopen(sdpath.c_str(), "w");
    if (file != nullptr) 
    {
        size_t written_len = fwrite(data, sizeof(uint8_t), data_len, file);
        fclose(file);
        
        if (written_len == data_len)
        {
            written = true;
            ESP_LOGD(MODULE_TAG, "File %s written.", sdpath.c_str());
        }
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Failed to open file %s for writing.", sdpath.c_str());
    }

    return written;
}

bool SDCard::append(const std::string& filepath, uint8_t* data, size_t data_len)
{
    ESP_LOGD(MODULE_TAG, "Appending to file in SDCard.");
    bool appended = false;

    std::string sdpath = MOUNT_POINT + filepath; 

    ESP_LOGD(MODULE_TAG, "Opening file %s.", sdpath.c_str());
    FILE *file = fopen(sdpath.c_str(), "a");
    if (file != nullptr) 
    {
        size_t appended_len = fwrite(data, sizeof(uint8_t), data_len, file);
        fclose(file);
        
        if (appended_len == data_len)
        {
            appended = true;
            ESP_LOGD(MODULE_TAG, "File %s written.", sdpath.c_str());
        }
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Failed to open file %s for writing.", sdpath.c_str());
    }

    return appended;
}

}
