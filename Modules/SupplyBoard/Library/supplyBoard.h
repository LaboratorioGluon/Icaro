#ifndef SUPPLYBOARD_H__
#define SUPPLYBOARD_H__

#include <stdint.h>
#include <stm32l0xx_hal.h>
#include "supplyerr.h"


#define hI2cSlave s_hI2cSlave

extern "C" I2C_HandleTypeDef getSlave();

class SupplyBoard
{
public:
    typedef struct
    {
        struct
        {
            uint8_t enable;
            uint8_t samplingTime;
        } adc;

        struct
        {
            uint8_t enable;
            uint8_t address;
            uint8_t i2cSpeed;
            uint8_t i2cMode;
        }i2c;

        struct 
        {
            uint8_t enable3v3;
            uint8_t enable5v;
            uint8_t enableInput;
            uint8_t enableBypass;
        } currentSense;        
        
    }SupplyBoardInit;

    SupplyBoard();

    void initHw(SupplyBoardInit initConfig);

    /** 
     * Initialize the I2C slave peripheral
     * and instantiate the hI2cSlave handle.
     * 
     * @param address I2C address of the slave device.
     * @note The address is 7-bit, so the maximum value is 127.
     * @note The address is shifted left by 1 bit to match the I2C peripheral
     *       address format.
     */
    SupplyErr initI2cSlave(uint8_t address);

    /**
     * @brief Set the I2C slave handle if you want to provide it from outside.
     * and keep the initialization of the peripheral.
     * 
     * @param i2cHandle Pointer to the I2C handle.
     **/
    //void setI2cSlave(I2C_HandleTypeDef *i2cHandle) { hI2cSlave = *i2cHandle; }

    /**
     * @brief Get the I2C slave handle.
     * 
     * @return Pointer to the I2C handle.
     */
    //static I2C_HandleTypeDef* getI2cSlave() { return &hI2cSlave; }

    SupplyErr i2cSlaveStart();


    /**
     * @return Temperatura en mCº. i.e.: 25700 -> 25.7º
     */
    uint16_t getSensorTemp();

    ADC_HandleTypeDef hAdc;
    
    static I2C_HandleTypeDef s_hI2cSlave;

private:

    void initAdc();
    uint8_t isAdcInitialized = 0;




};

#endif //SUPPLYBOARD_H__