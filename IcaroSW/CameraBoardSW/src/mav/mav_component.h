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
            // printf("Sending MAVLink message: %d bytes\n", mavBuffer_length);
            // printf("MAVLink message: ");
            // for (size_t i = 0; i < mavBuffer_length; i++)
            // {
            //     printf("%02X ", mavBuffer[i]);
            // }
            // printf("\n");
            return link->write(reinterpret_cast<char*>(mavBuffer), mavBuffer_length);
        }
        return -1;
    }
};

}