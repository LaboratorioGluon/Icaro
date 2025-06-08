#pragma once 

namespace Device
{

class IDevice
{
public:
    IDevice() = default;
    virtual ~IDevice() = default;

    virtual bool configure() = 0;
    virtual bool initialize() = 0;

    virtual bool isAvailable() const = 0;
};

}
