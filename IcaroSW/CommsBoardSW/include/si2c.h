#ifndef SI2C_H__
#define SI2C_H__

#include <freertos/FreeRTOS.h>
#include <driver/i2c_slave.h>



void si2c_init();

void si2c_setCallbacks(i2c_slave_received_callback_t receiveCallback, i2c_slave_request_callback_t requestCallback);

#endif //SI2C_H__