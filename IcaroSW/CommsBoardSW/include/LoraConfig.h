#include "driver/gpio.h"

/***********************************************************************************
* DEFAULT LORA SETTINGS
*  
* Normally needs no change
************************************************************************************/
#define LORA_BANDWIDTH 62.5         // Do not change, change LORA_MODE instead
#define LORA_SPREADFACTOR 8          // Do not change, change LORA_MODE instead
#define LORA_CODERATE 8              // Do not change, change LORA_MODE instead
#define LORA_SYNCWORD 0x12           // Default syncword
#define LORA_POWER 10                // in dBm between 2 and 17. 10 = 10mW (recommended)
#define LORA_CURRENTLIMIT 0
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0

#define LORA_PAYLOAD_ID  "ICARO-01"  // This will show on Sondehub. Payload ID for LoRa protocol. CHANGE THIS!
#define LORA_PREFIX "$$"             // Prefix for "Telemetry". Some older LoRa software does not accept a prefix of more than 2x "$"
//#define LORA_FREQUENCY  432.662      // in MHz
#define LORA_FREQUENCY  434.0f      // in MHz
#define LORA_REPEATS 1               // number of LoRa transmits during a cycle
#define LORA_LOOPTIME 15            // Transmit LoRa every xx seconds

/***********************************************************************************
* TRANSMISSIONS SETTINGS
*
* Change if needed
************************************************************************************/
#define MESSAGE_LENGTH 100     // Maximum length of telemetry line to send

constexpr gpio_num_t PIN_NSS   = GPIO_NUM_5; //Same than CS
constexpr gpio_num_t PIN_DIO0  = GPIO_NUM_39;
constexpr gpio_num_t PIN_RESET = GPIO_NUM_14;
constexpr gpio_num_t PIN_BUSY  = GPIO_NUM_NC; // Not used in this sketch for sx1278
constexpr gpio_num_t PIN_DIO1  = GPIO_NUM_NC; // Not used in this sketch for sx1278

constexpr gpio_num_t PIN_SCK  = GPIO_NUM_18;
constexpr gpio_num_t PIN_MISO = GPIO_NUM_19;
constexpr gpio_num_t PIN_MOSI = GPIO_NUM_23;

// Lora settings
struct TLoRaSettings
{
    float Frequency = LORA_FREQUENCY;
    float Bandwidth = LORA_BANDWIDTH;
    uint8_t SpreadFactor = LORA_SPREADFACTOR;
    uint8_t CodeRate = LORA_CODERATE;
    uint8_t SyncWord = LORA_SYNCWORD;
    uint8_t Power = LORA_POWER;
    uint8_t CurrentLimit = LORA_CURRENTLIMIT;
    uint16_t PreambleLength =  LORA_PREAMBLELENGTH;
    uint8_t Gain = LORA_GAIN;
};

// Lora message
struct TLoraMessage
{
    const char aPrefix[3] = LORA_PREFIX;
    const char PayloadID[9] = LORA_PAYLOAD_ID;
    uint32_t MessageCounter = 0;
    float Latitude;
    float Longitude;
    float InternalTemp;
    float BattVoltage;
    int GPSHours;
    int GPSMinutes;
    int GPSSeconds;
    long GPSAltitude;
    unsigned int GPSSatellites;
    unsigned int GPSHeading;
};