#pragma once

#include "../icam.h"

namespace Device::Camera
{

class OV2640 : public ICamera
{
private:
    bool initialized;
public:
    OV2640();
    virtual ~OV2640() = default;
    
    bool initialize() override;
    bool configure() override;

    bool isAvailable() const override;
    
    frame_t* grabFrame() override;
    void freeFrame(frame_t* frame) override;
};

}
