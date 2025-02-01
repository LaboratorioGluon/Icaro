#include "ims_ifI2c.h"

#include <driver/i2c_master.h>

ImsIfI2c::ImsIfI2c( ImsIfI2c::Config config)
{

}

uint8_t ImsIfI2c::init()
{
    return 0;
}

uint8_t ImsIfI2c::send( 
        const uint8_t dst, 
        const uint8_t cmd,
        const uint8_t * const data,
        const uint32_t size)
{
return 0;
}

uint8_t ImsIfI2c::read( 
        const uint8_t dst,
        const uint8_t cmd,
        uint8_t &data, 
        uint8_t &len )
{
return 0;
}

