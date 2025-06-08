#ifndef IMS_IFI2C_H__
#define IMS_IFI2C_H__

#include "../ims_interface.h"

class ImsIfI2c final: public ImsInterface{
public:
    typedef struct{
        uint8_t sda;
        uint8_t scl;
    } Config;

public:

    ImsIfI2c( ImsIfI2c::Config config);

    uint8_t init() override;
    
    uint8_t send( 
        const uint8_t dst, 
        const uint8_t cmd,
        const uint8_t * const data,
        const uint32_t size);
    
    uint8_t read( 
        const uint8_t dst,
        const uint8_t cmd,
        uint8_t &data, 
        uint8_t &len );

};

#endif //IMS_IFI2C_H__