
extern "C"{
  #include <stm32l0xx_hal.h>
}
#include <supplyBoard.h>

I2C_HandleTypeDef c_hI2cSlave;

extern SupplyBoard supplyBoard;

extern "C" void I2C1_IRQHandler(void)
{
   I2C_HandleTypeDef *hI2c = &supplyBoard.s_hI2cSlave;
  
  if (hI2c->Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR))
  {
    HAL_I2C_ER_IRQHandler(hI2c);
  }
  else
  {
    HAL_I2C_EV_IRQHandler(hI2c);
  }
}

extern uint32_t start, end;

extern "C" void DMA1_Channel1_IRQHandler(void)
{
  DMA_HandleTypeDef *hDma = &supplyBoard.hDma;

  end = TIM2->CNT;
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(hDma);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}
