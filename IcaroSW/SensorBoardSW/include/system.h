#ifndef SYSTEM_H__
#define SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <driver/i2c_master.h>
#include "led.h"

extern i2c_master_dev_handle_t i2cDeviceBme280;
extern i2c_master_dev_handle_t i2cDeviceAds1115;
extern i2c_master_dev_handle_t i2cDeviceBmi160;
extern i2c_master_dev_handle_t i2cSupplyDev;


void system_init();



#ifdef __cplusplus
}
#endif
#endif //SYSTEM_H__