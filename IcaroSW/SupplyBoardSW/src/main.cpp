#define HAL_I2C_MODULE_ENABLED
#include <stm32l0xx_hal.h>

#include "supplyBoard.h"
#include "dataMap.h"
#include <stdio.h>
#include <string.h>

#define I2C_DATA_SIZE 11

#define STM_SPEED_PIN GPIO_PIN_15
#define STM_SPEED_PORT GPIOA

uint32_t start, end;
TIM_HandleTypeDef tim2;
DMA_HandleTypeDef hDma = {};
I2C_HandleTypeDef hI2c;

SupplyBoard supplyBoard;

uint8_t newData = 0;
uint8_t rcvData[10];
uint8_t rcvDataCount = 0;



/*
SupplyBoard::SupplyBoardInit sbConfig = 
{
    .periph = {
        .adc.samplingTime = ADC_SAMPLETIME_160CYCLES_5
    },
    .sensorTemp.enable = 0
};*/

extern "C" void HAL_I2C_ListenCpltCallback (I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}

void SystemClock_Config(void)
{
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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

uint16_t i2cData[I2C_DATA_SIZE];
uint8_t *i2cDataMem = (uint8_t*)dataMap;
uint16_t nextReadDataIndex=2; // Index of the next data to be read
uint8_t nextReadDataByte = 0;
uint32_t nextReadDataOffset = 0;

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode){
	UNUSED(AddrMatchCode);
    
	if(TransferDirection == I2C_DIRECTION_TRANSMIT){ // They are writing to us
		HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rcvData, 1, I2C_FIRST_FRAME);
        rcvDataCount = 0;
        newData = 1;
    }
    else 
    {
		HAL_I2C_Slave_Sequential_Transmit_IT(hi2c, i2cDataMem+nextReadDataOffset, 1, I2C_NEXT_FRAME);
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    rcvDataCount++;

    if (rcvDataCount < 10)
    {
        
        if (rcvDataCount == 10-1)
        {
            HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rcvData+rcvDataCount, 1, I2C_LAST_FRAME);
        }
        else
        {
            HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rcvData+rcvDataCount, 1, I2C_NEXT_FRAME);
        }
    }
    else
    {
        
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    nextReadDataOffset++;
    if (nextReadDataOffset >= DATAMAP_SIZE)
    {
        nextReadDataOffset = 0;
    }
    HAL_I2C_Slave_Sequential_Transmit_IT(hi2c, i2cDataMem+nextReadDataOffset, 1, I2C_NEXT_FRAME);
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    uint32_t error = HAL_I2C_GetError(hi2c);
    if(error == HAL_I2C_ERROR_AF)
    {
        //nextReadDataOffset = rcvData[0];

        // 
        if (rcvDataCount == 1)
        {
            nextReadDataOffset = rcvData[0];
        }
        else if (rcvDataCount == 2)
        {
            if( rcvData[0] == DATAMAP_CONFIG_OFFSET)
            {
                dataMap[DATAMAP_CONFIG_OFFSET] = rcvData[1];
            }
        }
         
    }
    HAL_I2C_EnableListen_IT(hi2c);
}

void DMA_Finish(struct __DMA_HandleTypeDef * hdma)
{
    volatile int a = 0;
    a = 20;
}

int main()
{

    for(uint32_t i = 0; i < I2C_DATA_SIZE; i++)
    {
        i2cData[i] = i*2;
    }

    HAL_Init();

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_I2C1_CLK_ENABLE();

    SystemClock_Config();

    tim2.Instance = TIM2;
    tim2.Init.Prescaler = 9;
    tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim2.Init.Period=0xFFFFFFFF;
    tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&tim2);

    /** GPIO Standard **/
    GPIO_InitTypeDef initGpio;
    initGpio.Pin = STM_SPEED_PIN;
    initGpio.Mode = GPIO_MODE_OUTPUT_PP;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    initGpio.Alternate = 0;

    HAL_GPIO_Init(GPIOA, &initGpio);
    HAL_GPIO_WritePin(GPIOA, STM_SPEED_PIN, GPIO_PIN_SET);

    initGpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &initGpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
    /** ADC **/

    initGpio.Pin = GPIO_PIN_6 | GPIO_PIN_5;
    initGpio.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &initGpio);

    ADC_HandleTypeDef adcHandle;
    adcHandle.Instance = ADC1;
    adcHandle.State = 0;
    adcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
    adcHandle.Init.Resolution = ADC_RESOLUTION12b;
    adcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //adcHandle.Init.ScanConvMode = DISABLE;
    adcHandle.Init.ScanConvMode = ENABLE;
    //adcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adcHandle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
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

    adcChan.Channel = ADC_CHANNEL_0;
    adcChan.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_1;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_2;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_3;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_4;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_5;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    adcChan.Channel = ADC_CHANNEL_6;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);

    /*adcChan.Channel = ADC_CHANNEL_5;
    adcChan.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&adcHandle, &adcChan);*/

    HAL_ADCEx_Calibration_Start(&adcHandle, ADC_SINGLE_ENDED);
    volatile uint16_t adcRawValue;

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

    __HAL_LINKDMA(&adcHandle, DMA_Handle, hDma);
    
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /** UART **/

    initGpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    initGpio.Mode = GPIO_MODE_AF_PP;
    initGpio.Alternate = GPIO_AF0_USART2;

    HAL_GPIO_Init(GPIOB, &initGpio);

    /*UART_HandleTypeDef uartHandle;
    uartHandle.Instance = USART2;
    uartHandle.Init.BaudRate = 115200;
    uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartHandle.Init.Mode = UART_MODE_TX_RX;
    uartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    uartHandle.Init.Parity = UART_PARITY_NONE;
    uartHandle.Init.StopBits = UART_STOPBITS_1;
    uartHandle.Init.WordLength = UART_WORDLENGTH_8B;

    HAL_UART_Init(&uartHandle);*/


    /** I2C **/

    /*initGpio.Pin = GPIO_PIN_10 | GPIO_PIN_9;
    initGpio.Alternate = GPIO_AF1_I2C1;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Mode = GPIO_MODE_AF_OD;
    initGpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;    
    HAL_GPIO_Init(GPIOA, &initGpio);
    

    hI2c.Instance = I2C1;
    hI2c.Mode = HAL_I2C_MODE_SLAVE;
    hI2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hI2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hI2c.Init.OwnAddress1 = 0x58<<1;
    hI2c.Init.Timing = 0x00503D58;
    hI2c.Init.OwnAddress2 = 0;
    hI2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hI2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hI2c.State = HAL_I2C_STATE_RESET;
    HAL_I2C_Init(&hI2c);

    HAL_I2CEx_ConfigAnalogFilter(&hI2c, I2C_ANALOGFILTER_ENABLE);
    HAL_I2CEx_ConfigDigitalFilter(&hI2c, 0);
    HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_IRQn);*/
    
    supplyBoard.initI2cSlave(0x58);
    supplyBoard.i2cSlaveStart();

    //HAL_I2C_EnableListen_IT(&hI2c);

    //uint16_t test = 10;
    //HAL_I2C_Slave_Receive_IT(&hI2c, (uint8_t*)&test, 2);

    supplyBoard.hAdc = adcHandle;
    volatile uint16_t temp;
    uint8_t buf[100];
    uint16_t adcData[100];
    memset(adcData, 0, 100*sizeof(uint16_t));
    HAL_TIM_Base_Start(&tim2);
    
    volatile uint16_t tempSensor;

    volatile uint16_t adcValues[7];
    
    for(;;)
    {

        /*HAL_ADC_Start(&adcHandle);
        HAL_ADC_PollForConversion(&adcHandle, HAL_MAX_DELAY);
        sense3v3 = HAL_ADC_GetValue(&adcHandle);
        sense5v = HAL_ADC_GetValue(&adcHandle);
        senseIn = HAL_ADC_GetValue(&adcHandle);
        senseBypass = HAL_ADC_GetValue(&adcHandle);
        tempSensor = HAL_ADC_GetValue(&adcHandle);

        tempSensor = (tempSensor - 620)*81;

        (void)sense3v3;
        (void)sense5v;
        (void)senseIn;
        (void)senseBypass;
        (void)tempSensor;
        //temp = supplyBoard.getSensorTemp();
        dataMap[DATAMAP_TEMP_OFFSET  ] = (tempSensor>>8) & 0xFF;
        dataMap[DATAMAP_TEMP_OFFSET+1] = tempSensor & 0xFF;*/

        HAL_ADC_Start_DMA(&adcHandle, (uint32_t*)adcValues, 7);

        //...

        HAL_ADC_PollForConversion(&adcHandle, HAL_MAX_DELAY);
        HAL_ADC_Stop_DMA(&adcHandle);



        tempSensor = (adcValues[6] - 620)*81;
        dataMap[DATAMAP_TEMP_OFFSET  ] = tempSensor & 0xFF;
        dataMap[DATAMAP_TEMP_OFFSET+1] = (tempSensor>>8) & 0xFF;



        HAL_Delay(500);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, dataMap[DATAMAP_CONFIG_OFFSET] & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        //HAL_Delay(1000);
        //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);


    }
    return 0;
}