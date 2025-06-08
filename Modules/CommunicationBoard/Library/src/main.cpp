#include "Lora.h"

extern "C" void app_main(void);

Lora LoraModule;

void app_main(void) 
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  printf("Start\n");

  //Initialize radio
  LoraModule.ResetRadio();
  LoraModule.Init();

  //Start loop
  for(;;){
    for (int i=1; i <= LORA_REPEATS; i++)
    {
      LoraModule.BuildMessage();
      LoraModule.SendMessage();
    }
    vTaskDelay(pdMS_TO_TICKS(LORA_LOOPTIME * 1000));
  }
}