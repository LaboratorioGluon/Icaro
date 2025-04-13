
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
