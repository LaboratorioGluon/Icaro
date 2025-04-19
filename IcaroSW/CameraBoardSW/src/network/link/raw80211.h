#pragma once 

#include <libwifi.h>

#include "ilink.h"

#include "../mac.h"

namespace Network::Link
{

class Raw80211Link : public ILink
{
private:
    const mac_t src;
    const mac_t dest;

    constexpr static size_t MAX_PACKET_SIZE  = 1500;
    constexpr static size_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - sizeof(libwifi_data_frame_header);
    // std::array<uint8_t, MAX_PACKET_SIZE> packetBuffer;

    libwifi_data_frame_header generateDataHeader(mac_t src, mac_t dst, mac_t bssid, uint16_t duration, uint16_t fragment, uint16_t segment);

    bool connect();
    bool isAvailable() const;

public:
    Raw80211Link(mac_t dest, mac_t src);
    virtual ~Raw80211Link();

    size_t read(char* data, size_t length) override;
    size_t write(const char* data, size_t length) override;
};

}
