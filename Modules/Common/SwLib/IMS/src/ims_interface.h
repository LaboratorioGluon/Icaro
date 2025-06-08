#ifndef IMS_INTERFACE_H__
#define IMS_INTERFACE_H__

#include <stdint.h>

/**
 * Base interface class to derive the interface for spi/i2c/...
 */
class ImsInterface{
public:
    virtual uint8_t init() = 0;

    template<typename T, int N>
    uint8_t configRouter(T arr[N]);

    virtual uint8_t send( 
        const uint8_t dst, 
        const uint8_t cmd,
        const uint8_t * const data,
        const uint32_t size) = 0;
    
    virtual uint8_t read( 
        const uint8_t dst,
        const uint8_t cmd,
        uint8_t &data, 
        uint8_t &len ) = 0;

private:

};




#endif //IMS_INTERFACE_H__