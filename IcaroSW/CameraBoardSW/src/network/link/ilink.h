#pragma once 

#include <stdint.h>
#include <stddef.h>

namespace Network::Link
{

class ILink
{
public:
    ILink() = default;
    ~ILink() = default;
    
    virtual bool isAvailable () const = 0;

    virtual size_t read(uint8_t* data, size_t length) = 0;
    virtual size_t write(uint8_t* data, size_t length) = 0;
};

}
