#include "socket.h"

#include <esp_log.h>

#include <lwip/sockets.h>

namespace 
{
const char* MODULE_TAG = "SOCKET";
}

namespace Network::Link
{

SocketLink::SocketLink(int sock, sockaddr_in dest) : sock(sock), dest(dest)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_NONE);
    ESP_LOGD(MODULE_TAG, "Created socket: %d", sock);
}

SocketLink::~SocketLink()
{
    lwip_close(sock);
    ESP_LOGD(MODULE_TAG, "Closed socket: %d", sock);
}

bool SocketLink::connect()
{
    int err = lwip_connect(sock, (struct sockaddr *)&dest, sizeof(dest));
    if (err == 0) 
    {
        available = true;
        ESP_LOGI(MODULE_TAG, "Successfully connected");
    }
    else
    {
        available = false;
        ESP_LOGE(MODULE_TAG, "Socket unable to connect: errno %d", errno);
    }
    return available;
}

bool SocketLink::isAvailable() const
{
    return available;
}

size_t SocketLink::read(char* data, size_t length)
{
    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);
    ssize_t len = lwip_recvfrom(sock, data, length, 0, (struct sockaddr *)&source_addr, &socklen);

    // Error occurred during receiving
    if (len < 0) {
        available = false;
        ESP_LOGE(MODULE_TAG, "Error reading from socket: errno %d", errno);
    }
    return len;
}

size_t SocketLink::write(const char* data, size_t length)
{
    ssize_t err = lwip_sendto(sock, data, length, 0, (struct sockaddr *)&dest, sizeof(dest));
    if (err < 0) {
        available = false;
        ESP_LOGE(MODULE_TAG, "Error writting to socket: errno %d", errno);
    }
    return err;
}

}