#pragma once 

#include <stdint.h>
#include <stddef.h>

namespace Network::Link
{

class ILink
{
public:
    ILink() = default;
    virtual ~ILink() = default;
    
    virtual bool connect() = 0;

    virtual bool isAvailable() const = 0;

    virtual size_t read(char* data, size_t length) = 0;
    virtual size_t write(const char* data, size_t length) = 0;
};

}
