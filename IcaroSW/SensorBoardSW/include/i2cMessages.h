#ifndef I2C_MESSAGES_H__
#define I2C_MESSAGES_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c_master.h>


typedef struct {
    uint8_t id;
    uint8_t status;
    uint16_t v5;
    uint16_t vbatt;
    uint16_t i33;
    uint16_t i5;
    uint16_t temp;
    uint8_t config;
} i2cmessages_data;

void i2cmessages_init(i2c_master_dev_handle_t bus);
uint8_t i2cmessage_read(i2cmessages_data *data);
uint8_t i2cmessage_set5v(uint8_t isOn);


#ifdef __cplusplus
}
#endif

#endif // I2C_MESSAGES_H__