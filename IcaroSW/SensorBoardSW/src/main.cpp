
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include "system.h"
#include <esp_timer.h>

#include "dataMapSupply.h"

#if 0
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define SDA_PIN 25
#define SCL_PIN 26

static const char *TAG = "i2cscanner";

void task(void *ignore)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.scl_io_num = SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);

    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    while (1)
    {
        esp_err_t res;
        printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
        printf("00:         ");
        for (uint8_t i = 3; i < 0x78; i++)
        {
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
            i2c_master_stop(cmd);
    
            res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
            if (i % 16 == 0)
                printf("\n%.2x:", i);
            if (res == 0)
                printf(" %.2x", i);
            else
                printf(" --");
            i2c_cmd_link_delete(cmd);
        }
        printf("\n\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main()
{
    // Start task
    xTaskCreatePinnedToCore(task, TAG, configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}

#else

#include "classBmi160.h"
#include "bme280.h"
#include <driver/i2c_master.h>
#include "bz251.h"
#include "sdcard.h"

extern "C" {
    #include "i2cMessages.h"    
    #include "ADS1115.h"
}


/** SENSORS **/

Bz251Data bz251Data;

TaskHandle_t taskGPS;


void coreAThread(void *arg)
{

    ESP_LOGE("GPS", "Starting coreA");
    uart_flush(UART_NUM_2);
    for(;;)
    {
        bz251.read();
        bz251.getData(bz251Data);
        
        ESP_LOGI("GPS", "Valid: %d\nlatitude\t%.8f\nlongitude\t%.8f\naltitude\t%.2f\nspeed\t%.8f\nsats\t%u\ndate\t%u/%u/%u\ntime\t%u:%u",
                        bz251Data.valid,
                        bz251Data.latitude,
                        bz251Data.longitude,
                        bz251Data.altitude,
                        bz251Data.speedKmh,
                        bz251Data.satellites,
                        bz251Data.day,bz251Data.month, bz251Data.year,
                        bz251Data.hour, bz251Data.minute
                    );

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

extern "C" void app_main() {
    
    sdCard.init();
    
    ESP_LOGE("MAIN", "Icaro Sonde Started");
    sdCard.logEvent("Icaro Sonde Started\n");
    
    
    system_init();
    sensors_init();

    // Status LED Update
    led_setDelay(LED_ALWAYS_ON);
    int32_t ledDelay = LED_ALWAYS_ON;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    i2cmessages_data supplyData;

    // Test comms
    esp_err_t status = i2cmessage_test();
    if (status != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to read i2c Secondaries");
        sdCard.logEvent("Failed to read i2c Secondaries\n");
        led_setDelay(100);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    
    for(;;)
    {

        // Read Supply Board data
        i2cmessage_read(&supplyData);

        ESP_LOGE("MAIN", "Supply Data: ID: %d, Status: %d, V5: %d, VBatt: %d, I33: %d, I5: %d, Temp: %d, Config: %d",
                 supplyData.id,
                 supplyData.status,
                 supplyData.v5,
                 supplyData.vbatt,
                 supplyData.i33,
                 supplyData.i5,
                 supplyData.temp,
                 supplyData.config);

        sensorData.timestamp = esp_timer_get_time(); // Get timestamp in milliseconds
        sensorData.v5voltage = supplyData.v5 ; // Convert to Volts
        sensorData.v3v3current = supplyData.i33 ; // Convert to Amperes
        sensorData.v5current = supplyData.i5 ; // Convert to Amperes
        sensorData.vin = supplyData.vbatt; // Convert to Volts
        sensorData.supplyTemp = supplyData.temp; // Convert to Celsius


        // Temperature, humidity and pressure
        bme280_get_sensor_data(BME280_ALL, &sensorData.bme, &bme280);

        // IMU
        bmi160.getData(sensorData.acc, sensorData.gyr);

        // ADS1115 (PT100)
        sensors_getPt100(sensorData.pt100_1, sensorData.pt100_2);
    
        // GPS
        bz251.getData(sensorData.gps);
        
        // Store to SD Card
        debugSensorData();
        sdCard.logData(&sensorData);

        // Send data to Comms Board
        i2cmessage_sendToLora(&sensorData);

        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000)); // Wait 1 second
        ledDelay = (ledDelay == LED_ALWAYS_ON) ? 0 : LED_ALWAYS_ON; // Toggle LED delay
        led_setDelay(ledDelay); // Update LED delay
        
    }


#if 0
    
    //xTaskCreatePinnedToCore(coreAThread, "core_A", 4096, NULL, 3, &taskGPS, 0);

    esp_err_t result;

    uint32_t i2cRecv;
    uint16_t supplyTemp;

    uint8_t addr = 0x4;

    uint8_t addr_onoff = DATAMAP_CONFIG_OFFSET;
    uint8_t onoff[2] = {DATAMAP_CONFIG_OFFSET, 0x00};

    
    for(;;)
    {
        //addr = DATAMAP_TEMP_OFFSET;
        addr = DATAMAP_V5_OFFSET;
        esp_err_t err = i2c_master_transmit(i2cSupplyDev, &addr, 1, 1000);
        if (err == ESP_OK)
        {
            err = i2c_master_receive(i2cSupplyDev, (uint8_t*)&supplyTemp, 2, 1000);
            ESP_LOGE("MAIN", "Temperatura supply: %d mDegrees", supplyTemp);

        }
        else
        {
            ESP_LOGE("MAIN", "I2C Tx Error: %d", err);
        }

        /*if ( onoff[1] == 0x00)
        {
            onoff[1] = 0x01;
        }
        else
        {
            onoff[1] = 0x00;
        }

        esp_err_t err2 = i2c_master_transmit(i2cSupplyDev, onoff, 2, 1000);
        if (err2 != ESP_OK)
        {
            ESP_LOGE("MAIN", "I2C Tx Error: %d", err2);
        }
        else
        {
            ESP_LOGE("MAIN", "I2C Tx Success");   
        }*/

        // Request single ended on pin AIN0
        ADS1115_request_diff_AIN0_AIN1(); // all functions except for get_conversion_X return 'esp_err_t' for logging

        // Check conversion state - returns true if conversion is complete
        while (!ADS1115_get_conversion_state())
            vTaskDelay(pdMS_TO_TICKS(5)); // wait 5ms before check again

        // Return latest conversion value
        result = ADS1115_get_conversion();
        ESP_LOGE("MAIN", "Conversion Value: %d", result);

        ESP_LOGE("MAIN", "Hello World!");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }


    vTaskDelay(pdMS_TO_TICKS(2000));


    /*i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);*/


    // Print settings
    ESP_LOGE("MAIN", "Settings: %d %d %d %d %d", settings.osr_h, settings.osr_p, settings.osr_t, settings.filter, settings.standby_time);


    bme280_data data;
    for(;;)
    {
        ESP_LOGE("MAIN", "Loop");
        //ESP_LOGE("MAIN", "Init: %d", bme280_init(&dev));
        //ESP_LOGE("MAIN", "Init: %d", bme280_init(&dev));
        bme280_get_sensor_data(BME280_ALL, &data, &dev);
        ESP_LOGE("MAIN", "Temp: %.2f", data.temperature);
        ESP_LOGE("MAIN", "Hum: %.2f", data.humidity);
        ESP_LOGE("MAIN", "Pres: %.2f", data.pressure);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }


    for(;;)
    {
        bmi.getData(acc, gyr);
        printf("$%.2f %.2f %.2f %.2f;\n", acc.x, acc.y, acc.z, acc.time);
        //ESP_LOGI("main", "Acc: %.2f %.2f %.2f %.2f", acc.x, acc.y, acc.z, acc.time);
        //ESP_LOGI("main", "Gyr: %.2f %.2f %.2f %.2f", gyr.x, gyr.y, gyr.z, gyr.time);
        //ESP_LOGI("main", "Hello World!");
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelay(2);

    }
#endif  
}

#endif
