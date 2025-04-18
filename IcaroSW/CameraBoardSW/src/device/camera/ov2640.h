#pragma once

#include "../icam.h"

namespace Device::Camera
{

class OV2640 : public ICamera
{
    bool initialized;
public:
    OV2640();
    ~OV2640() = default;
    
    bool initialize() override;
    bool configure() override;

    bool isAvailable() override;
    
    frame_t* takePicture() override;
    void freeBuffer(frame_t* frame) override;
};

}
