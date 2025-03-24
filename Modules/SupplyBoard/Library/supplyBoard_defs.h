#ifndef SUPPLYBOARD_DEFS_H__
#define SUPPLYBOARD_DEFS_H__

#include <stdint.h>
#include <stm32l0xx_hal.h>

typedef struct{
    uint32_t pin;
    GPIO_TypeDef * port;
} pinDef;

enum {
    IO_CURRENT_SENSE_3v3 = 0
} functionalPin;

constexpr pinDef supplyBoardIO[] = {
    [IO_CURRENT_SENSE_3v3] = { GPIO_PIN_3, GPIOA}
};

#endif //SUPPLYBOARD_DEFS_H__