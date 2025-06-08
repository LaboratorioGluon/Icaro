#include "Lora.h"
#include "driver/gpio.h"

static const char *TAG = "LORA";

Lora::Lora() {}

void Lora::Init()
{
    // Configure reset gpio
    gpio_config_t config;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (1 << PIN_RESET);
    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&config);

    config.pin_bit_mask = (1<< GPIO_NUM_15);
    gpio_config(&config);
    gpio_set_level(GPIO_NUM_15, 0); 

    config.pin_bit_mask = (1<< GPIO_NUM_13);
    gpio_config(&config);
    gpio_set_level(GPIO_NUM_13, 0); 
    gpio_set_level(GPIO_NUM_15, 1);

    // Initialize radio
    printf("[LoRA] Initializing ...\n");
    //radio.setRfSwitchPins(GPIO_NUM_13, GPIO_NUM_15);
    /*int16_t state = radio.begin(LoRaSettings.Frequency,
                                LoRaSettings.Bandwidth,
                                LoRaSettings.SpreadFactor,
                                LoRaSettings.CodeRate,
                                LoRaSettings.SyncWord,
                                LoRaSettings.Power,
                                LoRaSettings.PreambleLength, 
                                3.3f);*/
    int16_t state = radio.begin();
    radio.setBandwidth(LoRaSettings.Bandwidth);
    radio.setSpreadingFactor(LoRaSettings.SpreadFactor);
    radio.setCodingRate(LoRaSettings.CodeRate);
    radio.setSyncWord(LoRaSettings.SyncWord);
    radio.setPreambleLength(LoRaSettings.PreambleLength);
    
    
    radio.setOutputPower(20);
    //radio.explicitHeader();
    //radio.forceLDRO(false);
    //radio.setCRC(true);

    if (state != RADIOLIB_ERR_NONE)  printf("[LoRA] Init failed, code %d\n", state);
    else                             printf("[LoRA] Init done\n");
}

void Lora::ResetRadio()
{
    gpio_set_level(PIN_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void Lora::BuildMessage()
{
    uint32_t Count;
    uint32_t CRC;

    //TODO: dummy data, fill with real values
    LoraMessage.GPSHours      = 16;
    LoraMessage.GPSMinutes    = 51;
    LoraMessage.GPSSeconds    = 31;
    LoraMessage.Latitude      = 38.22298;
    LoraMessage.Longitude     = -0.55862;
    LoraMessage.GPSAltitude   = 10714;
    LoraMessage.GPSSatellites = 24;
    LoraMessage.GPSHeading    = 233;
    LoraMessage.InternalTemp  = -36.0;
    LoraMessage.BattVoltage   = 3.63;

    sprintf( Message,
             "%s%s,%ld,%02d:%02d:%02d,%.5f,%.5f,%ld,%u,%u,%.1f,%.2f",
             LoraMessage.aPrefix,
             LoraMessage.PayloadID,
             LoraMessage.MessageCounter++,
             LoraMessage.GPSHours, LoraMessage.GPSMinutes, LoraMessage.GPSSeconds,   
             LoraMessage.Latitude,
             LoraMessage.Longitude,
             LoraMessage.GPSAltitude,
             LoraMessage.GPSSatellites,
             LoraMessage.GPSHeading,
             LoraMessage.InternalTemp,
             LoraMessage.BattVoltage);

    Count = strlen(Message);

    // Calc CRC
    CRC = 0xffff; // Seed

    for (uint32_t i = strlen(LoraMessage.aPrefix); i < Count; i++)
    {   
        // For speed, repeat calculation instead of looping for each bit
        CRC ^= (((uint32_t)Message[i]) << 8);
        for (uint32_t j=0; j<8; j++)
        {
            if (CRC & 0x8000)
            CRC = (CRC << 1) ^ 0x1021;
            else
            CRC <<= 1;
        }
    }

    Message[Count++] = '*';
    Message[Count++] = Hex((CRC >> 12) & 15);
    Message[Count++] = Hex((CRC >> 8) & 15);
    Message[Count++] = Hex((CRC >> 4) & 15);
    Message[Count++] = Hex(CRC & 15);
    Message[Count++] = '\n';  
    Message[Count++] = '\0';

    printf("[Lora] Message: %s\n", Message);
}

void Lora::SendMessage()
{
    printf("[Lora] Sending message...");
    //int16_t state = radio.transmit(Message);
    int16_t state = radio.transmit((uint8_t*)&sensorData, sizeof(sensorData));
  
    if(state == RADIOLIB_ERR_NONE) printf("OK\n");
    else                           printf("FAILED, code %d\n", state);
}

char Lora::Hex(uint8_t Character)
{
    uint8_t HexTable[] = "0123456789ABCDEF";
    return HexTable[Character];
}