#include "raw80211.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include "../mac.h"
#include "ilink.h"

namespace 
{
const char*           MODULE_TAG       = "RAW80211";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_NONE;
}

namespace Network::Link
{

Raw80211Link::Raw80211Link(mac_t src, mac_t dest, RAW_LINK_ID linkID) : src(src), dest(dest), linkID(linkID), messageID(0), packetBuffer{0}
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);
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
    messageID++;

    int pos = 0;

    // Fill Frame 802.11 header
    fillDataHeader(reinterpret_cast<libwifi_data_frame_header*>(&packetBuffer[pos]), src, dest, dest, 0, 0, 0);
    pos += sizeof(libwifi_data_frame_header);

    // Fill Frame LLC header
    fillLLCHeader(reinterpret_cast<libwifi_logical_link_ctrl*>(&packetBuffer[pos]));
    pos += sizeof(libwifi_logical_link_ctrl);

    const size_t fragmentStartingPos = pos;
    constexpr size_t MAX_FRAGMENT_SIZE = MAX_PAYLOAD_SIZE - sizeof(fragment_header_t);
    const size_t totalFragmentNum = (length / MAX_FRAGMENT_SIZE) + 1;
    
    size_t totalBytesSent = 0;
    size_t bytesLeft = length;

    size_t fragmentCount = 0;
    size_t fragmentLength = 0;
    while (fragmentCount < totalFragmentNum)
    {
        // Restart position for fragment data
        pos = fragmentStartingPos;
        
        // Calculate fragment data
        fragmentLength = std::min(bytesLeft, MAX_FRAGMENT_SIZE);
        
        // Fill Frame Fragment header
        fillFragmentHeader(reinterpret_cast<fragment_header_t*>(&packetBuffer[pos]), linkID, messageID, totalFragmentNum, fragmentCount, fragmentLength);
        pos += sizeof(fragment_header_t);

        // Fill payload data
        memcpy(&packetBuffer[pos], &data[totalBytesSent], fragmentLength);
        pos += fragmentLength;

        // Send data
        int retry = 0;
        bool fragmentSent = false;
        esp_err_t err = ESP_OK;
        while (retry < 5)
        {
            const size_t bytesToSend = pos;
            err = esp_wifi_80211_tx(WIFI_IF_STA, packetBuffer.data(), bytesToSend, false);
            if (err == ESP_OK)
            {
                fragmentSent = true;
                break;
            }
            retry++;
        }

        if (err == ESP_OK && fragmentSent)
        {
            // Update sending status data
            bytesLeft -= fragmentLength;
            totalBytesSent += fragmentLength;
            fragmentCount++;
        }
        else
        {
            ESP_LOGE(MODULE_TAG, "Failed to send: %s\n", esp_err_to_name(err));
            break;
        }
    }

    return totalBytesSent;
}

void Raw80211Link::fillDataHeader(libwifi_data_frame_header* frame, mac_t src, mac_t dst, mac_t bssid, uint16_t duration, uint16_t fragment, uint16_t segment)
{
    frame->duration = duration;
    frame->frame_control.version = 0;
    frame->frame_control.type = TYPE_DATA;
    frame->frame_control.subtype = SUBTYPE_DATA;
    frame->frame_control.flags.to_ds = 0;
    frame->frame_control.flags.from_ds = 0;
    frame->frame_control.flags.more_frags = 0;
    frame->frame_control.flags.retry = 0;
    frame->frame_control.flags.power_mgmt = 0;
    frame->frame_control.flags.more_data = 0;
    frame->frame_control.flags.protect = 0;
    frame->frame_control.flags.ordered = 0;
    memcpy(frame->addr1, dst.data(), MAC_LENGTH);
    memcpy(frame->addr2, src.data(), MAC_LENGTH);
    memcpy(frame->addr3, bssid.data(), MAC_LENGTH);
    constexpr int FRAGMENT_MASK = 0x000F;
    frame->seq_control.fragment_number = (fragment & FRAGMENT_MASK);
    constexpr int SEQUENCE_MASK = 0x0FFF;
    frame->seq_control.sequence_number = (fragment & SEQUENCE_MASK);
}

void Raw80211Link::fillLLCHeader(libwifi_logical_link_ctrl* frame)
{
    constexpr uint8_t SNAP_PROTOCOL = 0xAA;
    frame->dsap    = SNAP_PROTOCOL;
    frame->ssap    = SNAP_PROTOCOL;
    constexpr uint8_t UNNUMERED_DATA = 0x03;
    frame->control = UNNUMERED_DATA;
    constexpr char TEST_OUI[3] = {0x00, 0x00, 0x00};
    frame->oui[0]  = TEST_OUI[0];
    frame->oui[1]  = TEST_OUI[1];
    frame->oui[2]  = TEST_OUI[2];
    constexpr uint16_t EXPERIMENTAL_ETHERTYPE = 0x88B5;
    frame->type    = EXPERIMENTAL_ETHERTYPE;
}

void Raw80211Link::fillFragmentHeader(fragment_header_t* frame, RAW_LINK_ID link_id, uint16_t message_id, uint16_t total_frags, uint16_t frag_index, uint16_t payload_len)
{
    frame->link_id     = static_cast<uint8_t>(link_id);
    frame->message_id  = message_id;
    frame->total_frags = total_frags;
    frame->frag_index  = frag_index;
    frame->payload_len = payload_len;
}

}
