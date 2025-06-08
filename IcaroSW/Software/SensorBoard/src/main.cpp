#include <freertos/FreeRTOS.h>

#include "Interfaces_ESP/ims_ifI2c.h"
#include "Interfaces_ESP/ims_iftest.h"
#include "ims_messages.h"
#include "ims.h"

#include <driver/gpio.h>
#include <inttypes.h>


uint8_t inBuffer[200] = {0};
uint8_t outBuffer[200] = {0};

ImsIfTest::Config testConfig =
{
    .buffIn = inBuffer,
    .buffOut = outBuffer
};

ImsIfTest ifTest(testConfig);

ImsIfI2c::Config i2cConfig = {
    .sda = (uint8_t)0,
    .scl = (uint8_t)0,
};

ImsIfI2c i2cTest(i2cConfig);


Ims ims;

namespace ImsMessage{

    typedef struct  {
        uint8_t status5v5;
        uint8_t status3v3;
    } IMS_PACK SupplyStatus;

}

extern "C" int app_main(void)
{

    ims.init(ifTest);


    ImsMessage::SupplyStatus status;
    status.status3v3= 10;
    status.status5v5 = 1;

    uint8_t src;
    uint64_t time;

    ImsMessage::SupplyStatus status2 = {};

    while(1)
    {
        ims.send<ImsMessage::SupplyStatus>(0, 0x01, status);

        for ( int i = 0; i < 100; i++ )
        {
            printf("0x%02X ", inBuffer[i]);
        }

        ims.receive<ImsMessage::SupplyStatus>(src, 0x10, status2);

        printf("Status received: %" PRIu8 ", %" PRIu8 , status2.status3v3, status2.status5v5);
        printf("\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}