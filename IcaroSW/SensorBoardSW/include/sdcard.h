#ifndef SDCARD_H__
#define SDCARD_H__

#include <freertos/FreeRTOS.h>
#include "sensors.h"


class SDCard
{
public:

    typedef struct{
        gpio_num_t clk;
        gpio_num_t cmd;
        gpio_num_t data0;
        gpio_num_t data1;
        gpio_num_t data2;
        gpio_num_t data3;
    } SDCardConfig;


    SDCard(SDCardConfig config);
    ~SDCard(){};

    esp_err_t init();
    
    esp_err_t logData(SensorData *data);
    esp_err_t logEvent(const char *fmt, ...);
    
private:

    SDCardConfig sdConfig;

    FILE * dataFile;
    FILE * eventFile;
};

#endif // SDCARD_H__