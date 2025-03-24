#include "supplyBoard.h"


void SupplyBoard::initHw(SupplyBoardInit initConfig)
{
    if( initConfig.sensorTemp.enable )
    {

    }
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