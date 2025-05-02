#include "raw80211.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include "../mac.h"
#include "ilink.h"

namespace 
{
const char* MODULE_TAG = "RAW80211";
}

namespace Network::Link
{

Raw80211Link::Raw80211Link(mac_t src, mac_t dest) : src(src), dest(dest), packetBuffer{0}
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_NONE);
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
        int pos = 0;

        // Fill Frame 802.11 header
        fillDataHeader(reinterpret_cast<libwifi_data_frame_header*>(&packetBuffer[pos]), src, dest, dest, 0, 0, 0);
        pos += sizeof(libwifi_data_frame_header);

        // Fill Frame LLC header
        fillLLCHeader(reinterpret_cast<libwifi_logical_link_ctrl*>(&packetBuffer[pos]));
        pos += sizeof(libwifi_logical_link_ctrl);

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

}
