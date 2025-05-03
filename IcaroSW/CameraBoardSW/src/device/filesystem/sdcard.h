#pragma once

#include <string>
#include <stdint.h>

#include "../ifilesystem.h"

#include <sd_protocol_types.h>

namespace Device::FileSystem
{

class SDCard : public IFileSystem
{
private:
    bool initialized;
    sdmmc_card_t *card;

    bool mount();

public:
    SDCard();
    virtual ~SDCard() = default;
    
    bool configure() override;
    bool initialize() override;

    bool isAvailable() const override;

    bool write(const std::string& filepath, uint8_t* data, size_t length) override;
    bool append(const std::string& filepath, uint8_t* data, size_t data_len) override;
};

}
