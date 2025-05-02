#include "raw80211.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include "../mac.h"
#include "ilink.h"

namespace 
{
const char* MODULE_TAG = "RAW80211";

// constexpr size_t PACKET_MAX_SIZE = 1500;
// uint8_t staticPacketBuffer[PACKET_MAX_SIZE] = {0};

// libwifi_data_frame_header generateDataHeader(Network::mac_t src, Network::mac_t dst, Network::mac_t bssid, uint16_t duration, uint16_t fragment, uint16_t segment)
// {
//     libwifi_data_frame_header frame;
//     frame.duration = duration;
//     frame.frame_control.version = 0;
//     frame.frame_control.type = TYPE_DATA;
//     frame.frame_control.subtype = SUBTYPE_DATA;
//     frame.frame_control.flags.to_ds = 0;
//     frame.frame_control.flags.from_ds = 0;
//     frame.frame_control.flags.more_frags = 0;
//     frame.frame_control.flags.retry = 0;
//     frame.frame_control.flags.power_mgmt = 0;
//     frame.frame_control.flags.more_data = 0;
//     frame.frame_control.flags.protect = 0;
//     frame.frame_control.flags.ordered = 0;
//     memcpy(frame.addr1, dst.data(), Network::MAC_LENGTH);
//     memcpy(frame.addr2, src.data(), Network::MAC_LENGTH);
//     memcpy(frame.addr3, bssid.data(), Network::MAC_LENGTH);
//     constexpr int FRAGMENT_MASK = 0x000F;
//     frame.seq_control.fragment_number = (fragment & FRAGMENT_MASK);
//     constexpr int SEQUENCE_MASK = 0x0FFF;
//     frame.seq_control.sequence_number = (fragment & SEQUENCE_MASK);
//     return frame;
// }

// void initPacket()
// {
//     memset(staticPacketBuffer, 0, PACKET_MAX_SIZE);

//     libwifi_data_frame_header f802header = 
//         generateDataHeader(Network::DEFAULT_MAC, Network::BROADCAST_MAC, Network::BROADCAST_MAC, 0, 0, 0);
   
//     // Fill Frame 802.11 header
//     int pos = 0;
//     memcpy(&staticPacketBuffer[pos], &f802header, sizeof(f802header));
//     pos += sizeof(f802header);
    
//     // Fill dummy data
//     uint8_t dbyte = 0;
//     for (int i=pos; i<sizeof(staticPacketBuffer); i++)
//     {
//         staticPacketBuffer[i] = dbyte;
//         dbyte++;
//     }
// }

}

namespace Network::Link
{

Raw80211Link::Raw80211Link(mac_t src, mac_t dest) : src(src), dest(dest), packetBuffer{0}
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_NONE);
    // initPacket();
}

Raw80211Link::~Raw80211Link()
{

}

bool Raw80211Link::connect()
{
    /* No connection needed */
    return true;
}

bool Raw80211Link::isAvailable() const
{
    /* Always available */
    return true;
}

size_t Raw80211Link::read(char* data, size_t length)
{
    // TODO: Implement?
    return 0;
}

size_t Raw80211Link::write(const char* data, size_t length)
{
    size_t bytesSent = 0;
    if (length <= MAX_PAYLOAD_SIZE)
    {
        libwifi_data_frame_header f80211header = 
            generateDataHeader(src, dest, dest, 0, 0, 0);

        // Fill Frame 802.11 header
        int pos = 0;
        memcpy(&packetBuffer[pos], &f80211header, sizeof(f80211header));
        pos += sizeof(f80211header);

        // Fill payload data
        memcpy(&packetBuffer[pos], data, length);
        pos += length;

        const size_t len = pos;
        esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, packetBuffer.data(), len, false);
        if (err == ESP_OK)
        {
            bytesSent = len;
        }
        else
        {
            ESP_LOGE(MODULE_TAG, "Failed to send: %s\n", esp_err_to_name(err));
        }
    }
    else
    {
        ESP_LOGE(MODULE_TAG, "Payload size too big.");
    }
    return bytesSent;

    // ESP_LOGI(MODULE_TAG, "Sending static packet");
    // const size_t len = sizeof(staticPacketBuffer);
    // esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, staticPacketBuffer, len, false);
    // if (err != ESP_OK) {
    //     printf("Failed to send: %s\n", esp_err_to_name(err));
    // }
    // return 0;
}

libwifi_data_frame_header Raw80211Link::generateDataHeader(mac_t src, mac_t dst, mac_t bssid, uint16_t duration, uint16_t fragment, uint16_t segment)
{
    libwifi_data_frame_header frame;
    frame.duration = duration;
    frame.frame_control.version = 0;
    frame.frame_control.type = TYPE_DATA;
    frame.frame_control.subtype = SUBTYPE_DATA;
    frame.frame_control.flags.to_ds = 0;
    frame.frame_control.flags.from_ds = 0;
    frame.frame_control.flags.more_frags = 0;
    frame.frame_control.flags.retry = 0;
    frame.frame_control.flags.power_mgmt = 0;
    frame.frame_control.flags.more_data = 0;
    frame.frame_control.flags.protect = 0;
    frame.frame_control.flags.ordered = 0;
    memcpy(frame.addr1, dst.data(), MAC_LENGTH);
    memcpy(frame.addr2, src.data(), MAC_LENGTH);
    memcpy(frame.addr3, bssid.data(), MAC_LENGTH);
    constexpr int FRAGMENT_MASK = 0x000F;
    frame.seq_control.fragment_number = (fragment & FRAGMENT_MASK);
    constexpr int SEQUENCE_MASK = 0x0FFF;
    frame.seq_control.sequence_number = (fragment & SEQUENCE_MASK);
    return frame;
}

}
