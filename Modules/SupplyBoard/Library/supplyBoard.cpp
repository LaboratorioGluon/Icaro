#include "supplyBoard.h"
#include <string.h>

extern "C" I2C_HandleTypeDef c_hI2cSlave;

I2C_HandleTypeDef SupplyBoard::s_hI2cSlave = {0};

SupplyBoard::SupplyBoard()
{
    memset(&hI2cSlave, 0, sizeof(hI2cSlave));
    isAdcInitialized = 0;

}

void SupplyBoard::initHw(SupplyBoardInit initConfig)
{
}

SupplyErr SupplyBoard::initI2cSlave(uint8_t address)
{
    c_hI2cSlave.ErrorCode = 0;
    memset(&hI2cSlave, 0, sizeof(hI2cSlave));
    HAL_StatusTypeDef status;

    GPIO_InitTypeDef initGpio;
    initGpio.Pin = GPIO_PIN_10 | GPIO_PIN_9;
    initGpio.Alternate = GPIO_AF1_I2C1;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Mode = GPIO_MODE_AF_OD;
    initGpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;    
    HAL_GPIO_Init(GPIOA, &initGpio);

    hI2cSlave.Instance = I2C1;
    hI2cSlave.Mode = HAL_I2C_MODE_SLAVE;
    hI2cSlave.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hI2cSlave.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hI2cSlave.Init.OwnAddress1 = address<<1;
    hI2cSlave.Init.Timing = 0x00503D58;
    hI2cSlave.Init.OwnAddress2 = 0;
    hI2cSlave.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hI2cSlave.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hI2cSlave.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hI2cSlave.State = HAL_I2C_STATE_RESET;
    status = HAL_I2C_Init(&hI2cSlave);

    if (status == HAL_OK)
    {
        status = HAL_I2CEx_ConfigAnalogFilter(&hI2cSlave, I2C_ANALOGFILTER_ENABLE);
    }

    if (status == HAL_OK)
    {
        status = HAL_I2CEx_ConfigDigitalFilter(&hI2cSlave, 0);
    }

    if (status == HAL_OK)
    {
        HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(I2C1_IRQn);
    }

    return status == HAL_OK ? SUPPLY_OK : SUPPLY_ERR;
}

uint16_t SupplyBoard::getSensorTemp()
{
    uint16_t adcRawData = 0;

    HAL_ADC_Start(&hAdc);
    HAL_ADC_PollForConversion(&hAdc, HAL_MAX_DELAY);
    adcRawData = HAL_ADC_GetValue(&hAdc);

    // T = (Vo - 500mV)/10mV 
    // 500mV = 155 [adc]
    // 10mV = 3.1 [adc]
    // 3.1*1000 (Para pasar a mCº) = 322.58 ~= 323
    // T = (Vo - 155)*323; 
    // 10bits:
    // return (adcRawData - 155)*323;
    // 12 bits:
    return (adcRawData - 620)*81;
}


void SupplyBoard::initAdc()
{
    if (isAdcInitialized)
        return;

    ADC_HandleTypeDef adcHandle;
    adcHandle.Instance = ADC1;
    adcHandle.State = 0;
    adcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
    adcHandle.Init.Resolution = ADC_RESOLUTION12b;
    adcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adcHandle.Init.ScanConvMode = DISABLE;
    adcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adcHandle.Init.LowPowerAutoWait = 0;
    adcHandle.Init.LowPowerAutoPowerOff = 0;
    adcHandle.Init.LowPowerAutoWait = 0;
    adcHandle.Init.ContinuousConvMode = DISABLE;
    adcHandle.Init.DiscontinuousConvMode = DISABLE;
    adcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle.Init.DMAContinuousRequests = DISABLE;
    adcHandle.Init.Overrun = DISABLE;
    adcHandle.Init.LowPowerFrequencyMode = DISABLE;
    adcHandle.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    adcHandle.Init.OversamplingMode = DISABLE;
    
    HAL_StatusTypeDef status = HAL_ADC_Init(&adcHandle);
    if (status == HAL_OK)
    {
        isAdcInitialized = 1;
    }
}

SupplyErr SupplyBoard::i2cSlaveStart()
{
    if (hI2cSlave.State == HAL_I2C_STATE_RESET)
    {
        return SUPPLY_ERR;
    }

    HAL_I2C_EnableListen_IT(&hI2cSlave);
    return SUPPLY_OK;
}