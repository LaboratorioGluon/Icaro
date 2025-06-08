#ifndef SUPPLYBOARD_DEFS_H__
#define SUPPLYBOARD_DEFS_H__

#include <stdint.h>
#include <stm32l0xx_hal.h>

typedef struct{
    uint32_t pin;
    GPIO_TypeDef * port;
} pinDef;

enum {
    IO_5V_ENABLE = 0,
    IO_BYPASS_ENABLE,
    IO_STM_SPEED
} functionalPin;

constexpr pinDef supplyBoardIO[] = {
    [IO_5V_ENABLE]     = { GPIO_PIN_15, GPIOC },
    [IO_BYPASS_ENABLE] = { GPIO_PIN_14, GPIOC },
    [IO_STM_SPEED]     = { GPIO_PIN_15, GPIOA }

};

#endif //SUPPLYBOARD_DEFS_H__