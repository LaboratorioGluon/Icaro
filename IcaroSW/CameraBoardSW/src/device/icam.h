#pragma once

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include "idevice.h"

namespace Device
{

struct frame_t
{
    uint8_t*       buf;       /*!< Pointer to the pixel data */
    size_t         len;       /*!< Length of the buffer in bytes */
    size_t         width;     /*!< Width of the buffer in pixels */
    size_t         height;    /*!< Height of the buffer in pixels */
    uint32_t       reserved;  /*!< Format of the pixel data */
    struct timeval timestamp; /*!< Timestamp since boot of the first DMA buffer of the frame */
};

class ICamera : public IDevice
{
public:
    ICamera() : IDevice() {};
    virtual ~ICamera() = default;

    virtual frame_t* takePicture() = 0;
    virtual void freeBuffer(frame_t* frame) = 0;
};

}
