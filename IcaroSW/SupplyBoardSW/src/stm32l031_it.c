
#include <stm32l0xx_hal.h>

void SysTick_Handler(void)
{
  HAL_IncTick();
}
