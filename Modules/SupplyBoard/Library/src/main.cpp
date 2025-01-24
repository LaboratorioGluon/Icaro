#include <stm32l0xx_hal.h>


struct{
    uint8_t a;
    uint8_t b;
} message1;


struct{
    uint32_t a;
    float b;
} message2;

extern "C" int main() {
    
    message2.a = 5;
    message2.b = 10.0f;
    ims.send(&message2, sizeof(message2));
    i2c, spi, uart..
    //
    
    message2 m2;
    ims.receiveMessage2(&m2);
    m2? -> .a = 5, .b = 10.0f


    return 0;
}