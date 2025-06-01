#ifndef LED_H__
#define LED_H__

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

#define LED_ALWAYS_ON 99999999

void led_init();
void led_setDelay(int32_t delay);


#endif // LED_H__