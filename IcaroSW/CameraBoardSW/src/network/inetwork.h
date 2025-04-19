#pragma once 

#include <stdint.h>
#include <stddef.h>

namespace Network
{

class INetwork
{
public:
    INetwork() = default;
    ~INetwork() = default;

    virtual bool initialize() = 0;
};

}
