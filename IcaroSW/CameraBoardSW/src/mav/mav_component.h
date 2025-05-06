#pragma once

#include <memory>

#include <esp_log.h>

#include <common/mavlink.h>

#include "../network/link/ilink.h"

namespace MAVLink
{

class MAVComponent
{
private:
    std::unique_ptr<Network::Link::ILink> link;
    uint8_t mavBuffer[MAVLINK_MAX_PACKET_LEN];

protected:
    MAVComponent(std::unique_ptr<Network::Link::ILink> link) : link(std::move(link)) {}
    virtual ~MAVComponent() = default;

    int send(const mavlink_message_t& mavMessage)
    {
        if (!link->isAvailable())
        {
            link->connect();
        }
        if (link->isAvailable())
        {
            size_t mavBuffer_length = mavlink_msg_to_send_buffer(mavBuffer, &mavMessage);
            return link->write(reinterpret_cast<char*>(mavBuffer), mavBuffer_length);
        }
        return -1;
    }
};

}