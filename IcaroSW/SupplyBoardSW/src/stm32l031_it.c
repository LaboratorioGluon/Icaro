
#include <stm32l0xx_hal.h>

void SysTick_Handler(void)
{
  HAL_IncTick();
}

extern uint32_t start, end;
extern TIM_HandleTypeDef tim2;
extern DMA_HandleTypeDef hDma;

void DMA1_Channel1_IRQHandler(void)
{
  end = TIM2->CNT;
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hDma);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

#if 0
extern I2C_HandleTypeDef hI2c;
void I2C1_IRQHandler(void)
{
  if (hI2c.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR))
  {
    HAL_I2C_ER_IRQHandler(&hI2c);
  }
  /*else if( hI2c.Instance->ISR & (I2C_FLAG_AF))
  {
    __HAL_I2C_CLEAR_FLAG(&hI2c, I2C_FLAG_AF);
    HAL_I2C_ErrorCallback(&hI2c);
  }*/
  else
  {
    HAL_I2C_EV_IRQHandler(&hI2c);
  }
}
  #endif