#pragma once

#include <memory>
#include <string>

#include <esp_netif.h>
#include <esp_wifi.h>

#include "link/ilink.h"

namespace Network
{

class WiFi
{
private:
    esp_netif_t* netif;
    wifi_init_config_t m_config;

    esp_event_handler_instance_t ip_event_handler;
    esp_event_handler_instance_t wifi_event_handler;

    bool connect();
    bool disconnect();

public:
    WiFi();
    virtual ~WiFi();

    bool initialize();

    std::unique_ptr<Link::ILink> createUDPLink(std::string ip, int port);
    std::unique_ptr<Link::ILink> createTCPLink(std::string ip, int port);
};

}