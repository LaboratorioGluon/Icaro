#include "ims_iftest.h"

#include <string.h>

ImsIfTest::ImsIfTest( ImsIfTest::Config config)
{
    _config = config;
}

uint8_t ImsIfTest::init()
{
    return 0;
}

uint8_t ImsIfTest::send( 
        const uint8_t dst, 
        const uint8_t cmd,
        const uint8_t * const data,
        const uint32_t size)
{
    memcpy(_config.buffIn, data, size);
    return 0; 
}

uint8_t ImsIfTest::read( 
        const uint8_t dst,
        const uint8_t cmd,
        uint8_t &data, 
        uint8_t &len )
{
    memcpy( (uint8_t*)&data, _config.buffIn, len);
    return 0;
}

