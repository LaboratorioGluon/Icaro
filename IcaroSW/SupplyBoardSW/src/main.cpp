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

SupplyBoard supplyBoard(DEFAULT_SUPPLY_HW);

uint8_t newData = 0;
uint8_t rcvData[10];
uint8_t rcvDataCount = 0;


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

uint8_t *i2cDataMem = (uint8_t*)dataMap;
uint8_t nextReadDataByte = 0;
uint32_t nextReadDataOffset = 0;

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode){
	UNUSED(AddrMatchCode);
    
	if(TransferDirection == I2C_DIRECTION_TRANSMIT){ // They are writing to us
		HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rcvData, 1, I2C_FIRST_AND_NEXT_FRAME);
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

/*
void DMA_Finish(struct __DMA_HandleTypeDef * hdma)
{
}
*/

int main()
{


    HAL_Init();


    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();


    //SystemClock_Config();
    supplyBoard.configureClock();

    tim2.Instance = TIM2;
    tim2.Init.Prescaler = 9;
    tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim2.Init.Period=0xFFFFFFFF;
    tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&tim2);

    /** GPIO Standard **/
    supplyBoard.initGpio();
    GPIO_InitTypeDef initGpio;
    /*initGpio.Pin = STM_SPEED_PIN;
    initGpio.Mode = GPIO_MODE_OUTPUT_PP;
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    initGpio.Alternate = 0;

    HAL_GPIO_Init(GPIOA, &initGpio);
    HAL_GPIO_WritePin(GPIOA, STM_SPEED_PIN, GPIO_PIN_SET);

    initGpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &initGpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);*/
    /** ADC **/

    supplyBoard.initAdcDma();

    /** UART **/
    supplyBoard.initUart(115200);

    /*initGpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
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

    HAL_UART_Init(&uartHandle);*/
    
    supplyBoard.initI2cSlave(0x58);
    supplyBoard.i2cSlaveStart();


    ADC_HandleTypeDef adcHandle = supplyBoard.hAdc;    

    uint16_t tempSensor;
    uint16_t adcValues[7];
    uint16_t values[7];
    
    for(;;)
    {

        HAL_NVIC_DisableIRQ(I2C1_IRQn);
            HAL_ADC_Start_DMA(&adcHandle, (uint32_t*)adcValues, 7);
            HAL_ADC_PollForConversion(&adcHandle, HAL_MAX_DELAY);
            HAL_ADC_Stop_DMA(&adcHandle);
        HAL_NVIC_EnableIRQ(I2C1_IRQn);


        /* Vin Voltage */
        values[0] = adcValues[0] * (3300.0f / 4096.0f) * (14.7f / 10.0f);

        /* 5V Voltage*/
        values[1] = adcValues[1] * 3300.0f / 4096.0f * 2.0f;

        /* 3v3 Current */
        values[2] = adcValues[2] * (3300.0f / 4096.0f) / (200.0f * 0.01f);

        /* 5v Current */
        values[3] = adcValues[3] * (3300.0f / 4096.0f) / (200.0f * 0.1f);
        
        /* In Current */ 
        // TODO: values[4]
        /* Bypass Current*/
        // TODO: values[5]

        dataMap[DATAMAP_V5_OFFSET] = values[1] & 0xFF;
        dataMap[DATAMAP_V5_OFFSET+1] = (values[1]>>8) & 0xFF;

        dataMap[DATAMAP_VBATT_OFFSET] = values[0] & 0xFF;
        dataMap[DATAMAP_VBATT_OFFSET+1] = (values[0]>>8) & 0xFF;

        dataMap[DATAMAP_I33_OFFSET] = values[2] & 0xFF;
        dataMap[DATAMAP_I33_OFFSET+1] = (values[2]>>8) & 0xFF;

        dataMap[DATAMAP_I5_OFFSET] = values[3] & 0xFF;
        dataMap[DATAMAP_I5_OFFSET+1] = (values[3]>>8) & 0xFF;


        tempSensor = (adcValues[6] - 620)*81;
        dataMap[DATAMAP_TEMP_OFFSET  ] = tempSensor & 0xFF;
        dataMap[DATAMAP_TEMP_OFFSET+1] = (tempSensor>>8) & 0xFF;

        HAL_Delay(500);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, dataMap[DATAMAP_CONFIG_OFFSET] & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);


    }
    return 0;
}