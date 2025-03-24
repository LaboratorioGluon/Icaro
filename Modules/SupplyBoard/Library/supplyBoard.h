#ifndef SUPPLYBOARD_H__
#define SUPPLYBOARD_H__

#include <stdint.h>
#include <stm32l0xx_hal.h>


class SupplyBoard
{
public:
    typedef struct
    {
        struct
        {
            struct 
            {
                uint8_t samplingTime;
            } adc;

            struct
            {
                uint8_t mode; // 0 = MASTER, 1 = SLAVE
                union 
                {
                    struct{
                        uint8_t addr;
                    }slave;
                };
                
            } i2c;
            

        } periph;

        struct 
        {
            uint8_t enable;
        } sensorTemp;

        struct 
        {
            uint8_t enable3v3;
            uint8_t enable5v;
            uint8_t enableInput;
            uint8_t enableBypass;
        } currentSense;        
        
    }SupplyBoardInit;

    SupplyBoard(){};

    void initHw(SupplyBoardInit initConfig);

    /**
     * @return Temperatura en mCº. i.e.: 25700 -> 25.7º
     */
    uint16_t getSensorTemp();

    ADC_HandleTypeDef hAdc;
private:

    void initAdc();

    uint8_t isAdcInitialized = 0;

};

#endif //SUPPLYBOARD_H__