#pragma once 

#include "ilink.h"

namespace Network::Link
{

class TCPLink : public ILink
{
private:

public:
    TCPLink();
    ~TCPLink();

    bool isAvailable() const override;

    bool connect();
    bool disconnect();

    size_t read(uint8_t* data, size_t length) override;
    size_t write(uint8_t* data, size_t length) override;
};

}
