#pragma once

#include <memory>

#include <esp_netif.h>
#include <esp_wifi.h>

#include "link/ilink.h"
#include "link/rawlinkids.h"

namespace Network
{

class WiFiRaw
{
private:
    esp_netif_t* netif;
    wifi_init_config_t m_config;
    
public:
    WiFiRaw();
    virtual ~WiFiRaw();

    bool initialize();

    std::unique_ptr<Link::ILink> create80211Link(Network::Link::RAW_LINK_ID linkID);
};

}