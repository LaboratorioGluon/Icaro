#pragma once

#include <string>
#include <stdint.h>

#include "idevice.h"

namespace Device
{

class IFileSystem : public IDevice
{
public:
    IFileSystem() : IDevice() {};
    virtual ~IFileSystem() = default;

    virtual bool write(const std::string& filepath, uint8_t* data, size_t length) = 0;
    virtual bool append(const std::string& filepath, uint8_t* data, size_t length) = 0;
};

}
