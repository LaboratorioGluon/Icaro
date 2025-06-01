#include "led.h"
#include <freertos/task.h>
#include <esp_log.h>

#define LED_PIN GPIO_NUM_27


void led_on();
void led_off();

TaskHandle_t ledTaskHandle = NULL;
static int32_t ledDelay = 0;
static QueueHandle_t delayQueue = NULL;

void ledControlTask( void * p)
{
    (void)p; // Unused parameter
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int32_t localDelay = 0;
    for(;;)
    {
        //ESP_LOGE("LED", "ledControlTask running with delay: %ld", ledDelay);
        //xQueueReceive(delayQueue, &localDelay, 0); // Wait indefinitely for a new delay valu
        localDelay = ledDelay; // Get the current delay value
        if(localDelay == 0)
        {
            //ESP_LOGE("LED", "LED is OFF");
            led_off();
            xQueueReceive(delayQueue, &localDelay, portMAX_DELAY); // Wait indefinitely for a new delay valu
        }
        else if (localDelay == LED_ALWAYS_ON)
        {
            //ESP_LOGE("LED", "LED is ON");
            led_on();
            xQueueReceive(delayQueue, &localDelay, portMAX_DELAY); // Wait indefinitely for a new delay valu
        }
        else
        {
            led_on();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(localDelay)); // LED on for localDelay ms
            led_off();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(localDelay)); // LED off for localDelay ms
        }
        
    }

}


void led_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    led_off(); // Ensure LED is off initially
    
    // Create a task to control the LED
    if (ledTaskHandle == NULL) {
        delayQueue = xQueueCreate(1, sizeof(int32_t)); // Create a queue to hold the delay value
        xTaskCreate(ledControlTask, "ledControlTask", 4000, NULL, 5, &ledTaskHandle);
    } else {
        // If the task already exists, reset the delay
        ledDelay = 0;
    }
}

void led_on()
{
    gpio_set_level(LED_PIN, 1); // Set LED pin high to turn it on
}

void led_off()
{
    gpio_set_level(LED_PIN, 0); // Set LED pin low to turn it off
}



void led_setDelay(int32_t delay)
{
    ledDelay = delay; // Set the delay for the LED
    xQueueSend(delayQueue, &ledDelay, portMAX_DELAY); // Send the new delay to the queue
    //xTaskNotifyGive(ledTaskHandle); // Notify the task to update the LED state
}
