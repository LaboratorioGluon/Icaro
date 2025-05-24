#include "sdcard.h"

#include <esp_log.h>
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>

namespace
{
const char*           MODULE_TAG       = "SDCARD";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_DEBUG;

const char* MOUNT_POINT = "/sdcard";

// Bus definition
constexpr int SD_BUS_WIDTH = 1; 
constexpr gpio_num_t SD_PIN_CLK = GPIO_NUM_39;
constexpr gpio_num_t SD_PIN_CMD = GPIO_NUM_38;
constexpr gpio_num_t SD_PIN_D0 = GPIO_NUM_40;

}

namespace Device::FileSystem
{

SDCard::SDCard() : IFileSystem(), initialized(false), card(nullptr)
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
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

bool SDCard::isAvailable() const  
{
    return (card != nullptr) && initialized;
}

bool SDCard::makedir(const std::string& dirpath)
{
    std::string sdpath = MOUNT_POINT + dirpath; 
    int ret = mkdir(sdpath.c_str(), 0775);
    if (ret == 0) 
    {
        ESP_LOGD(MODULE_TAG, "Created directory: %s", sdpath.c_str());
    }
    else
    {       
        if (errno == EEXIST) {
            ESP_LOGD(MODULE_TAG, "El directorio ya existe: %s\n", sdpath.c_str());
        } else {
            ESP_LOGE(MODULE_TAG, "Error al crear el directorio %s: errno %d\n", sdpath.c_str(), errno);
        }
    }
    return ret == 0;
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

bool exists(char* file_path)
{
    FILE *f = fopen(file_path, "r");
    if (!f) {
        return false;
    }
    fclose(f);
    return true;
}

uint32_t SDCard::findMaxImage() const 
{
    char file_path[64];
    
    for (int dir = 0; dir <= 99999; dir++)
    {
        constexpr int MAX_DIR_JPG = 999;
        snprintf(file_path, sizeof(file_path),  "%s/%05d/%03d.JPG", MOUNT_POINT, dir, MAX_DIR_JPG);
        if (exists(file_path)) {
            // File exists, continue with next directory
            ESP_LOGD(MODULE_TAG, "Directory %d full", dir);
            continue;
        }
        else
        {
            ESP_LOGD(MODULE_TAG, "Scanning dir %d", dir);
            // File does not exist, find highest file that exists
            int i = 0, f = MAX_DIR_JPG;
            int candidate = 999;
            
            while (i <= f)
            {
                int c = (i + f) / 2;
                snprintf(file_path, sizeof(file_path),  "%s/%05d/%03d.JPG", MOUNT_POINT, dir, c);
                if (exists(file_path))
                {
                    // File exists, check numbers above
                    i = c+1;
                }
                else
                {
                    // File does not exist, check numbers below
                    candidate = c;
                    f = c-1;
                }
            }
            return dir * 1000 + candidate;
        }
    }
    return 0;
}

}
