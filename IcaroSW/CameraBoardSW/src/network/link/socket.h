#pragma once 

#include "ilink.h"

#include <lwip/sockets.h>

namespace Network::Link
{

class SocketLink : public ILink
{
private:
    const int sock;
    const sockaddr_in dest;

    bool available;

public:
    SocketLink(int sock, sockaddr_in dest);
    virtual ~SocketLink();

    bool connect() override;

    bool isAvailable() const override;

    size_t read(char* data, size_t length) override;
    size_t write(const char* data, size_t length) override;
};

}
