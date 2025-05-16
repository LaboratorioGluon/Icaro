#include "i2cslave.h"

#include <string.h>
#include <math.h>

#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include <esp_err.h>

namespace
{
const char* MODULE_TAG = "I2C_SLAVE";
}

namespace InterBoards
{

I2CSlave::I2CSlave(i2c_port_t i2cPort, uint16_t slaveAddr, gpio_num_t sdaPin, gpio_num_t sclPin, uint32_t clkSpeed) :
    i2cPort(i2cPort)
{
    esp_log_level_set(MODULE_TAG, ESP_LOG_DEBUG);
    
    config = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = sdaPin,
        .scl_io_num = sclPin,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .slave = 
        {
            .addr_10bit_en = false,
            .slave_addr = 0x28,
            .maximum_speed = 100000,
        },
        .clk_flags = 0,
    };
}

bool I2CSlave::initialize()
{
    constexpr size_t BUFFER_SIZE = 256;
    ESP_ERROR_CHECK(i2c_param_config(i2cPort, &config));
    ESP_ERROR_CHECK(i2c_driver_install(i2cPort, I2C_MODE_SLAVE, BUFFER_SIZE, BUFFER_SIZE, 0));
    return true;
};

size_t I2CSlave::read(uint8_t* data, size_t length)
{
    size_t readlen = 0;
    int len = i2c_slave_read_buffer(i2cPort, data, length, pdMS_TO_TICKS(1000));
    if (len < 0)
    {
        len = 0;
    }
    readlen = len;
    return readlen;
}

// bool I2CSlave::write(uint8_t* data, size_t length)
// {
//     // ESP_ERROR_CHECK(i2c_slave_write(handle, data_buffer, buffer_size, &write_len, 1000));
// }

}