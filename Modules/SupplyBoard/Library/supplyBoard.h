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

    typedef struct{
        struct{
            float bypassOhm;
            float inputOhm;
            float v3_3Ohm;
            float v5Ohm;
            float inputGain;
            float regulatedGain;
        } currentSense;
        struct{
            float v5Gain;
            float inputGain;
        } voltageSense;
    } SupplyBoardHardware;

    SupplyBoard(SupplyBoardHardware hw);

    /**
     * @brief Configure the system clock.
     */
    void configureClock();

    /**
     * @brief Initialize the UART peripheral.
     * 
     * @param baudrate Baudrate for the UART communication.
     */
    void initUart(uint32_t baudrate);

    /**
     * @brief Initialize the ADC peripheral and DMA.
     * 
     * @note The ADC is configured to use DMA for data transfer.
     */
    void initAdcDma();

    void initGpio();

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

    void convertAdcTemperature(uint16_t adcValue, float *temperature);

    void updateAdcValues();

    void enable5vOutput(uint8_t enable);

    inline float convertAdcVoltage(uint16_t adcValue)
    {
        return (float)adcValue * vdda / 4095.0f;
    }


    ADC_HandleTypeDef hAdc;
    
    static I2C_HandleTypeDef s_hI2cSlave;
    static DMA_HandleTypeDef hDma;


private:

    uint8_t isAdcInitialized = 0;

    SupplyBoardHardware hwConfig;

    uint16_t adcRawData[7];

    struct {
        uint16_t T1_CAL;
        uint16_t T1_CAL_TEMP;
        uint16_t T2_CAL;
        uint16_t T2_CAL_TEMP;
    } tempCalibrationData;

    struct {
        uint16_t VREF_CAL_REF;
        float VREF_CAL_VALUE;
    } vddaCalibrationData;

    float vdda;

};


constexpr SupplyBoard::SupplyBoardHardware DEFAULT_SUPPLY_HW = {
    .currentSense = {
        .bypassOhm = 0.01f,
        .inputOhm = 0.01f,
        .v3_3Ohm = 0.1f,
        .v5Ohm = 0.1f,
        .inputGain = 0.1f,
        .regulatedGain = 0.1f
    },
    .voltageSense = {
        .v5Gain = 10.0f/20.0f,
        .inputGain = 4.7f/14.7f
    }
};

#endif //SUPPLYBOARD_H__