#include "ov2640.h"

#include <esp_log.h>
#include <esp_err.h>

#include <esp_camera.h>

namespace
{
const char* MODULE_TAG = "OV2640";

// Camera pins
constexpr int CAM_PIN_PWDN  = -1;
constexpr int CAM_PIN_RESET = -1;
constexpr int CAM_PIN_VSYNC = 6;
constexpr int CAM_PIN_HREF  = 7;
constexpr int CAM_PIN_PCLK  = 13;
constexpr int CAM_PIN_XCLK  = 15;
constexpr int CAM_PIN_SIOD  = 4;
constexpr int CAM_PIN_SIOC  = 5;
constexpr int CAM_PIN_D0    = 11;
constexpr int CAM_PIN_D1    = 9;
constexpr int CAM_PIN_D2    = 8;
constexpr int CAM_PIN_D3    = 10;
constexpr int CAM_PIN_D4    = 12;
constexpr int CAM_PIN_D5    = 18;
constexpr int CAM_PIN_D6    = 17;
constexpr int CAM_PIN_D7    = 16;
}

namespace Device::Camera
{

OV2640::OV2640() : ICamera(), initialized(false)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);    
}

bool OV2640::initialize()
{
    ESP_LOGD(MODULE_TAG, "Initializing OV2640.");

    camera_config_t config;

    config.pin_pwdn  = CAM_PIN_PWDN;
    config.pin_reset = CAM_PIN_RESET;

    config.pin_xclk = CAM_PIN_XCLK;
    config.pin_sccb_sda = CAM_PIN_SIOD;
    config.pin_sccb_scl = CAM_PIN_SIOC;

    config.pin_d7 = CAM_PIN_D7;
    config.pin_d6 = CAM_PIN_D6;
    config.pin_d5 = CAM_PIN_D5;
    config.pin_d4 = CAM_PIN_D4;
    config.pin_d3 = CAM_PIN_D3;
    config.pin_d2 = CAM_PIN_D2;
    config.pin_d1 = CAM_PIN_D1;
    config.pin_d0 = CAM_PIN_D0;
    config.pin_vsync = CAM_PIN_VSYNC;
    config.pin_href = CAM_PIN_HREF;
    config.pin_pclk = CAM_PIN_PCLK;

    constexpr int MEGA = 1000000; 
    config.xclk_freq_hz = 20 * MEGA;           // Default 20MHz (EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode)
    config.ledc_timer = LEDC_TIMER_0;
    config.ledc_channel = LEDC_CHANNEL_0;
    
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_VGA;        // Note: Do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.
    
    config.jpeg_quality = 12;                 // Note: 0-63, for OV series camera sensors, lower number means higher quality
    config.fb_count = 4;                      // Note: When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    config.fb_location = CAMERA_FB_IN_PSRAM;
    // config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.grab_mode      = CAMERA_GRAB_LATEST;

    esp_err_t err = esp_camera_init(&config);
    if (err == ESP_OK)
    {
        initialized = true;
        ESP_LOGI(MODULE_TAG, "Camera OV2640 initialized.");
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Failed to initialize OV2640.");
    }

    return initialized;
}

bool OV2640::configure()
{
    /* Nothing to configure */
    return true;
}

bool OV2640::isAvailable()
{
    return initialized;
}

frame_t* OV2640::grabFrame()
{
    ESP_LOGD(MODULE_TAG, "Taking picture with OV2640.");
    camera_fb_t *pic = esp_camera_fb_get();
    return reinterpret_cast<frame_t*>(pic);
}

void OV2640::freeFrame(frame_t* frame)
{
    ESP_LOGD(MODULE_TAG, "Freeing buffer for OV2640.");
    esp_camera_fb_return(reinterpret_cast<camera_fb_t*>(frame));
}

}
