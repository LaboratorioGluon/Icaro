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

    virtual bool makedir(const std::string& dirpath) = 0;

    virtual bool write(const std::string& filepath, uint8_t* data, size_t length) = 0;
    virtual bool append(const std::string& filepath, uint8_t* data, size_t length) = 0;

    virtual uint32_t findMaxImage() const = 0;
};

}
