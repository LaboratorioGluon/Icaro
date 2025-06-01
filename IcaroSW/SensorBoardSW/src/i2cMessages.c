#include "i2cMessages.h"
#include <esp_log.h>

#define DATAMAP_ID_OFFSET 0x0
#define DATAMAP_STATUS_OFFSET 0x1
#define DATAMAP_V5_OFFSET 0x2
#define DATAMAP_VBATT_OFFSET 0x4
#define DATAMAP_I33_OFFSET 0x6
#define DATAMAP_I5_OFFSET 0x8
#define DATAMAP_TEMP_OFFSET 0xA
#define DATAMAP_CONFIG_OFFSET 0xC

#define DATAMAP_SIZE 0xE

static i2c_master_dev_handle_t i2cBus;

void i2cmessages_init(i2c_master_dev_handle_t bus)
{
    i2cBus = bus;
}

uint8_t i2cmessage_read(i2cmessages_data *data)
{
    uint8_t addr = DATAMAP_ID_OFFSET;
    
    esp_err_t err = i2c_master_transmit_receive(i2cBus, &addr, 1,  (uint8_t*)data, DATAMAP_SIZE, 1000);
    if (err != ESP_OK) {
        ESP_LOGE("I2C_MESSAGE", "Read Error: %s", esp_err_to_name(err));
    }
    return err;
}

uint8_t i2cmessage_set5v(uint8_t isOn)
{
    uint8_t addr = DATAMAP_CONFIG_OFFSET;
    uint8_t data[2] = {DATAMAP_CONFIG_OFFSET, isOn ? 0x01 : 0x00};

    esp_err_t err = i2c_master_transmit(i2cBus, data, 2, 1000);
    if (err != ESP_OK) {
        ESP_LOGE("I2C_MESSAGE", "Set 5V Error: %s", esp_err_to_name(err));
    }
    return err;
}
