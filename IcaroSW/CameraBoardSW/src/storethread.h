#pragma once

#include <memory>

#include "device/icam.h"
#include "device/ifilesystem.h"

typedef struct
{
    std::shared_ptr<Device::ICamera> camera;
    std::shared_ptr<Device::IFileSystem> fs;
} storeThreadArg_t;

void storeThreadFunc (void* arg);
