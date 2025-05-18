#include "wifi.h"

#include <array>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_event.h>

#include <lwip/sockets.h>

#include "mac.h"
#include "link/socket.h"

#include "secrets.h" // Must be created and WIFI_SSID/WIFI_PASSWORD defined

namespace 
{
const char* MODULE_TAG = "WIFI";

// WiFi event control
static EventGroupHandle_t s_wifi_event_group = nullptr;

static const int WIFI_RETRY_ATTEMPT = 100;
static int wifi_retry_count = 0;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static void ip_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(MODULE_TAG, "Handling IP event, event code 0x%" PRIx32, event_id);

    ip_event_got_ip_t *event_ip;
    ip_event_got_ip6_t *event_ip6;
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
        event_ip = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(MODULE_TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case IP_EVENT_STA_LOST_IP:
        ESP_LOGI(MODULE_TAG, "Lost IP");
        break;
    case IP_EVENT_GOT_IP6:
        event_ip6 = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(MODULE_TAG, "Got IPv6: " IPV6STR, IPV62STR(event_ip6->ip6_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        ESP_LOGI(MODULE_TAG, "IP event not handled");
        break;
    }
}

static void wifi_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(MODULE_TAG, "Handling Wi-Fi event, event code 0x%" PRIx32, event_id);

    switch (event_id)
    {
    case (WIFI_EVENT_WIFI_READY):
        ESP_LOGI(MODULE_TAG, "Wi-Fi ready");
        break;
    case (WIFI_EVENT_SCAN_DONE):
        ESP_LOGI(MODULE_TAG, "Wi-Fi scan done");
        break;
    case (WIFI_EVENT_STA_START):
        ESP_LOGI(MODULE_TAG, "Wi-Fi started...");
        // ESP_LOGI(MODULE_TAG, "Wi-Fi started, connecting to AP...");
        // esp_wifi_connect();
        break;
    case (WIFI_EVENT_STA_STOP):
        ESP_LOGI(MODULE_TAG, "Wi-Fi stopped");
        break;
    case (WIFI_EVENT_STA_CONNECTED):
        ESP_LOGI(MODULE_TAG, "Wi-Fi connected");
        break;
    case (WIFI_EVENT_STA_DISCONNECTED):
        ESP_LOGI(MODULE_TAG, "Wi-Fi disconnected");
        if (wifi_retry_count < WIFI_RETRY_ATTEMPT) {
            ESP_LOGI(MODULE_TAG, "Retrying to connect to Wi-Fi network...");
            esp_wifi_connect();
            wifi_retry_count++;
        } else {
            ESP_LOGI(MODULE_TAG, "Failed to connect to Wi-Fi network");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        break;
    case (WIFI_EVENT_STA_AUTHMODE_CHANGE):
        ESP_LOGI(MODULE_TAG, "Wi-Fi authmode changed");
        break;
    default:
        ESP_LOGI(MODULE_TAG, "Wi-Fi event not handled");
        break;
    }
}

}

namespace Network
{

WiFi::WiFi() : netif(nullptr), m_config(WIFI_INIT_CONFIG_DEFAULT())
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_NONE);    
    s_wifi_event_group = xEventGroupCreate();
}

WiFi::~WiFi()
{
    esp_err_t ret = esp_wifi_stop();
    if (ret == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGE(MODULE_TAG, "Wi-Fi stack not initialized");
    }

    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(netif));
    esp_netif_destroy(netif);

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler));

    if (s_wifi_event_group) {
        vEventGroupDelete(s_wifi_event_group);
        s_wifi_event_group = nullptr;
    }
}


bool WiFi::initialize()
{
    ESP_LOGI(MODULE_TAG, "Initializing NVS.");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    netif = esp_netif_create_default_wifi_sta();
    if (netif == NULL) {
        ESP_LOGE(MODULE_TAG, "Failed to create default WiFi STA interface");
        return false;
    }

    ESP_LOGI(MODULE_TAG, "Initializing WiFi.");
    ESP_ERROR_CHECK(esp_wifi_init(&m_config));

    ESP_LOGV(MODULE_TAG, "WiFi changing mode.");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_LOGV(MODULE_TAG, "WiFi setting mac.");
    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, DEFAULT_MAC.data()));

    ESP_LOGV(MODULE_TAG, "WiFi starting.");
    ESP_ERROR_CHECK(esp_wifi_start());

    connect();

    return true;
}

bool WiFi::connect()
{
    bool connected = false;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_cb,
        NULL,
        &wifi_event_handler));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        ESP_EVENT_ANY_ID,
        &ip_event_cb,
        NULL,
        &ip_event_handler));

    wifi_config_t wifi_config = {
        .sta= {
            .ssid = "",
            .password= "" 
        }
    };

    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, WIFI_PASSWORD, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(MODULE_TAG, "Connecting to Wi-Fi network: %s", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_connect());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(MODULE_TAG, "Connected to Wi-Fi network: %s", (char*)wifi_config.sta.ssid);
        connected = true;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(MODULE_TAG, "Failed to connect to Wi-Fi network: %s", (char*)wifi_config.sta.ssid);
        connected = false;
    }

    return connected;
}

bool WiFi::disconnect()
{
    if (s_wifi_event_group) {
        vEventGroupDelete(s_wifi_event_group);
        s_wifi_event_group = nullptr;
    }

    return esp_wifi_disconnect() == ESP_OK;
}

std::unique_ptr<Link::ILink> WiFi::createUDPLink(std::string ip, int port)
{
    std::unique_ptr<Link::ILink> link = nullptr;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock >= 0)
    {
        struct timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        ESP_LOGI(MODULE_TAG, "UDP Socket created, sending to %s:%d", ip.c_str(), port);

        link = std::make_unique<Link::SocketLink>(sock, dest_addr);
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Unable to create socket: errno %d", errno);
    }
    return link;
}

std::unique_ptr<Link::ILink> WiFi::createTCPLink(std::string ip, int port)
{
    std::unique_ptr<Link::ILink> link = nullptr;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock >= 0)
    {
        struct timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        
        ESP_LOGI(MODULE_TAG, "TCP Socket created, sending to %s:%d", ip.c_str(), port);

        link = std::make_unique<Link::SocketLink>(sock, dest_addr);
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Unable to create socket: errno %d", errno);
    }
    return link;
}

}