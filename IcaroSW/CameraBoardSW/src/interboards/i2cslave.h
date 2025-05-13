#pragma once

#include <freertos/FreeRTOS.h>

#include <driver/i2c.h>

namespace InterBoards
{

class I2CSlave
{
private:
    i2c_port_t i2cPort;
    i2c_config_t config;

public:

    I2CSlave(i2c_port_t i2cPort, uint16_t slaveAddr, gpio_num_t sdaPin, gpio_num_t sclPin, uint32_t clkSpeed);
    virtual ~I2CSlave() = default;

    bool initialize();

    size_t read(uint8_t* data, size_t length);
};

}