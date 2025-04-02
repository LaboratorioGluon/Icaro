#ifndef __LORA_H__
#define __LORA_H__

#include "EspHal.h"
#include "RadioLib.h"
#include "LoraConfig.h"

class Lora{
public:

    Lora();

    void Init();
    void ResetRadio();
    void BuildMessage();
    void SendMessage();

private:

    EspHal* hal  = new EspHal(PIN_SCK, PIN_MISO, PIN_MOSI);
    SX1278 radio = new Module(hal, PIN_NSS, PIN_DIO0, PIN_RESET, PIN_DIO1);

    TLoRaSettings LoRaSettings;
    TLoraMessage  LoraMessage;

    char Message[MESSAGE_LENGTH];

    char Hex(uint8_t Character);

};

#endif //__LORA_H__