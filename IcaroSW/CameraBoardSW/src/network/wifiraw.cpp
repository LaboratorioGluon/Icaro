#include "wifiraw.h"

#include <array>
#include <string>
#include <string.h>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_netif.h>

#include "mac.h"
#include "link/raw80211.h"

namespace 
{
const char* MODULE_TAG = "WIFIRAW";

constexpr int DEFAULT_CHANNEL = 6;
}

namespace Network
{

WiFiRaw::WiFiRaw() : netif(nullptr), m_config(WIFI_INIT_CONFIG_DEFAULT())
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_VERBOSE);    
}

WiFiRaw::~WiFiRaw()
{
    ESP_ERROR_CHECK(esp_wifi_deinit());
}

bool WiFiRaw::initialize()
{
    ESP_LOGI(MODULE_TAG, "Initializing NVS.");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(MODULE_TAG, "Initializing WiFi(RAW).");
    ESP_ERROR_CHECK(esp_wifi_init(&m_config));

    // ESP_LOGV(MODULE_TAG, "WiFi changing storage.");
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_LOGV(MODULE_TAG, "WiFi changing mode.");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_LOGV(MODULE_TAG, "WiFi setting mac.");
    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, DEFAULT_MAC.data()));

    ESP_LOGV(MODULE_TAG, "WiFi setting rate.");
    ESP_ERROR_CHECK(esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_24M));

    ESP_LOGV(MODULE_TAG, "WiFi starting.");
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGV(MODULE_TAG, "WiFi setting bandwidth.");
    ESP_ERROR_CHECK(esp_wifi_set_band_mode(WIFI_BAND_MODE_2G_ONLY));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20));

    wifi_protocols_t protocols;
    protocols.ghz_2g = WIFI_PROTOCOL_11N;
    ESP_ERROR_CHECK(esp_wifi_set_protocols(WIFI_IF_STA, &protocols));

    ESP_LOGV(MODULE_TAG, "WiFi changing channel.");
    ESP_ERROR_CHECK(esp_wifi_set_channel(DEFAULT_CHANNEL, WIFI_SECOND_CHAN_NONE));

    ESP_LOGV(MODULE_TAG, "Disabling power save.");
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    return true;
}

std::unique_ptr<Link::ILink> WiFiRaw::create80211Link()
{
    return std::make_unique<Link::Raw80211Link>(DEFAULT_MAC, BROADCAST_MAC);
}

}