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

    typedef struct __attribute__((packed)) 
    {
        // uint16_t magic;
        uint16_t message_id;
        uint16_t total_frags;
        uint16_t frag_index;
        uint16_t payload_len;
        // uint16_t crc16;
    } fragment_header_t;
    
    constexpr static size_t MAX_PACKET_SIZE  = 1500;
    constexpr static size_t HEADERS_SIZE = sizeof(libwifi_data_frame_header) + sizeof(libwifi_logical_link_ctrl);
    constexpr static size_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - HEADERS_SIZE;
    std::array<uint8_t, MAX_PACKET_SIZE> packetBuffer;

    void fillDataHeader(libwifi_data_frame_header* frame, mac_t src, mac_t dst, mac_t bssid, uint16_t duration, uint16_t fragment, uint16_t segment);
    void fillLLCHeader(libwifi_logical_link_ctrl* frame);
    void fillFragmentHeader(fragment_header_t* frame, uint16_t message_id, uint16_t total_frags, uint16_t frag_index, uint16_t payload_len);

    bool connect();
    bool isAvailable() const;

public:
    Raw80211Link(mac_t dest, mac_t src);
    virtual ~Raw80211Link();

    size_t read(char* data, size_t length) override;
    size_t write(const char* data, size_t length) override;
};

}
