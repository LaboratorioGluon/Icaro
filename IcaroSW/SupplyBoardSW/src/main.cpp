#include <stm32l0xx_hal.h>

#include "supplyBoard.h"

#define STM_SPEED_PIN GPIO_PIN_15
#define STM_SPEED_PORT GPIOA

int main()
{
    HAL_Init();

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    /** GPIO Standard **/
    GPIO_InitTypeDef initGpio;
    initGpio.Pin = STM_SPEED_PIN;
    initGpio.Mode = GPIO_MODE_OUTPUT_PP;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    initGpio.Alternate = 0;

    HAL_GPIO_Init(GPIOA, &initGpio);
    HAL_GPIO_WritePin(GPIOA, STM_SPEED_PIN, GPIO_PIN_SET);

    /** ADC **/

    initGpio.Pin = GPIO_PIN_6;
    initGpio.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &initGpio);

    ADC_HandleTypeDef adcHandle;
    adcHandle.Instance = ADC1;
    adcHandle.State = 0;
    adcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
    adcHandle.Init.Resolution = ADC_RESOLUTION10b;
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
    adcHandle.Init.SamplingTime = ADC_SAMPLETIME_79CYCLES_5;
    adcHandle.Init.OversamplingMode = DISABLE;
    
    HAL_ADC_Init(&adcHandle);

    ADC_ChannelConfTypeDef adcChan;
    adcChan.Channel = ADC_CHANNEL_6;
    adcChan.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    HAL_ADCEx_Calibration_Start(&adcHandle, ADC_SINGLE_ENDED);
    volatile uint16_t adcRawValue;


    /** UART **/

    initGpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    initGpio.Mode = GPIO_MODE_AF_PP;
    initGpio.Alternate = GPIO_AF0_USART2;

    HAL_GPIO_Init(GPIOB, &initGpio);

    UART_HandleTypeDef uartHandle;
    uartHandle.Instance = USART2;
    uartHandle.Init.BaudRate = 115200;
    uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartHandle.Init.Mode = UART_MODE_TX_RX;
    uartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    uartHandle.Init.Parity = UART_PARITY_NONE;
    uartHandle.Init.StopBits = UART_STOPBITS_1;
    uartHandle.Init.WordLength = UART_WORDLENGTH_8B;

    HAL_UART_Init(&uartHandle);

    for(;;)
    {
        HAL_ADC_Start(&adcHandle);
        HAL_ADC_PollForConversion(&adcHandle, HAL_MAX_DELAY);
        adcRawValue = HAL_ADC_GetValue(&adcHandle);
        HAL_UART_Transmit(&uartHandle, (uint8_t*)"Hola Chat!\r\n", 14, HAL_MAX_DELAY);
        HAL_Delay(1000);
        HAL_GPIO_TogglePin(GPIOA, STM_SPEED_PIN);
    }
    return 0;
}