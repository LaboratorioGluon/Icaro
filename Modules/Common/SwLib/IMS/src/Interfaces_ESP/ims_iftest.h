#ifndef IMS_IFTEST_H__
#define IMS_IFTEST_H__

#include "../ims_interface.h"

class ImsIfTest final: public ImsInterface{
public:
    typedef struct{
        uint8_t * buffIn;
        uint8_t * buffOut;
    } Config;

public:

    ImsIfTest( ImsIfTest::Config config);

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

private:

    Config _config;
};

#endif //IMS_IFTEST_H__