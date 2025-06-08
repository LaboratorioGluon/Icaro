#include "supplyBoard.h"
#include "supplyBoard_defs.h"
#include <string.h>

extern "C" I2C_HandleTypeDef c_hI2cSlave;

I2C_HandleTypeDef SupplyBoard::s_hI2cSlave = {0};
DMA_HandleTypeDef SupplyBoard::hDma = {0};

SupplyBoard::SupplyBoard(SupplyBoardHardware hw) : hwConfig(hw)
{
    memset(&hI2cSlave, 0, sizeof(hI2cSlave));
    isAdcInitialized = 0;
}

void SupplyBoard::configureClock()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
    }
}

void SupplyBoard::initUart(uint32_t baudrate)
{
    GPIO_InitTypeDef initGpio;
    initGpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    initGpio.Mode = GPIO_MODE_AF_PP;
    initGpio.Alternate = GPIO_AF0_USART2;

    HAL_GPIO_Init(GPIOB, &initGpio);

    UART_HandleTypeDef uartHandle;
    uartHandle.Instance = USART2;
    uartHandle.Init.BaudRate = baudrate;
    uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartHandle.Init.Mode = UART_MODE_TX_RX;
    uartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    uartHandle.Init.Parity = UART_PARITY_NONE;
    uartHandle.Init.StopBits = UART_STOPBITS_1;
    uartHandle.Init.WordLength = UART_WORDLENGTH_8B;

    HAL_UART_Init(&uartHandle);
}

SupplyErr SupplyBoard::initI2cSlave(uint8_t address)
{
    __HAL_RCC_I2C1_CLK_ENABLE();

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
    hI2cSlave.Init.OwnAddress1 = address << 1;
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

void SupplyBoard::initGpio()
{
    /** GPIO Standard **/
    GPIO_InitTypeDef initGpio;
    initGpio.Pin = supplyBoardIO[IO_STM_SPEED].pin;
    initGpio.Mode = GPIO_MODE_OUTPUT_PP;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    initGpio.Alternate = 0;

    HAL_GPIO_Init(supplyBoardIO[IO_STM_SPEED].port, &initGpio);
    HAL_GPIO_WritePin(supplyBoardIO[IO_STM_SPEED].port, supplyBoardIO[IO_STM_SPEED].pin, GPIO_PIN_SET);

    initGpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &initGpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
}

void SupplyBoard::initAdcDma()
{
    if (isAdcInitialized)
        return;

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    // ADC_HandleTypeDef adcHandle;
    hAdc.Instance = ADC1;
    hAdc.State = 0;
    hAdc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
    hAdc.Init.Resolution = ADC_RESOLUTION12b;
    hAdc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hAdc.Init.ScanConvMode = ENABLE;
    hAdc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hAdc.Init.LowPowerAutoWait = 0;
    hAdc.Init.LowPowerAutoPowerOff = 0;
    hAdc.Init.LowPowerAutoWait = 0;
    hAdc.Init.ContinuousConvMode = DISABLE;
    hAdc.Init.DiscontinuousConvMode = DISABLE;
    hAdc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hAdc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hAdc.Init.DMAContinuousRequests = DISABLE;
    hAdc.Init.Overrun = DISABLE;
    hAdc.Init.LowPowerFrequencyMode = DISABLE;
    hAdc.Init.SamplingTime = ADC_SAMPLETIME_79CYCLES_5;
    hAdc.Init.OversamplingMode = DISABLE;

    HAL_StatusTypeDef status = HAL_ADC_Init(&hAdc);
    if (status == HAL_OK)
    {
        isAdcInitialized = 1;
    }

    ADC_ChannelConfTypeDef adcChan;

    adcChan.Channel = ADC_CHANNEL_0;
    adcChan.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_1;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_2;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_3;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_4;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_5;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    adcChan.Channel = ADC_CHANNEL_6;
    HAL_ADC_ConfigChannel(&hAdc, &adcChan);

    HAL_ADCEx_Calibration_Start(&hAdc, ADC_SINGLE_ENDED);

    hDma.Instance = DMA1_Channel1;
    hDma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hDma.Init.MemInc = DMA_MINC_ENABLE;
    hDma.Init.Mode = DMA_CIRCULAR;
    hDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hDma.Init.PeriphInc = DMA_PINC_DISABLE;
    hDma.Init.Priority = DMA_PRIORITY_MEDIUM;
    hDma.Init.Request = DMA_REQUEST_0;
    hDma.Init.Direction = DMA_PERIPH_TO_MEMORY;

    HAL_DMA_Init(&hDma);

    __HAL_LINKDMA(&hAdc, DMA_Handle, hDma);

    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
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

void enable5vOutput(uint8_t enable)
{
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, dataMap[DATAMAP_CONFIG_OFFSET] & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}