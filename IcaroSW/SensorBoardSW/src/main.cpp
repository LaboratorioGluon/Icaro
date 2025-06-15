
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include "system.h"
#include <esp_timer.h>

#include "dataMapSupply.h"


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

TaskHandle_t taskHandleGPS;


void taskGPS(void *arg)
{

    ESP_LOGE("GPS", "Starting coreA");
    uart_flush(UART_NUM_2);
    for(;;)
    {
        bz251.read();
        //bz251.getData(bz251Data);
        
        /*ESP_LOGI("GPS", "Valid: %d\nlatitude\t%.8f\nlongitude\t%.8f\naltitude\t%.2f\nspeed\t%.8f\nsats\t%u\ndate\t%u/%u/%u\ntime\t%u:%u",
                        bz251Data.valid,
                        bz251Data.latitude,
                        bz251Data.longitude,
                        bz251Data.altitude,
                        bz251Data.speedKmh,
                        bz251Data.satellites,
                        bz251Data.day,bz251Data.month, bz251Data.year,
                        bz251Data.hour, bz251Data.minute
                    );
        */
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

extern "C" void app_main() {
    
    vTaskDelay(pdMS_TO_TICKS(2000)); // Wait for system to stabilize
    sdCard.init();
    
    ESP_LOGE("MAIN", "Icaro Sonde Started");
    sdCard.logEvent("Icaro Sonde Started\n");
    
    
    esp_err_t status = system_init();
    if (status != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to read i2c Secondaries");
        sdCard.logEvent("Failed to read i2c Secondaries\n");
        led_setDelay(100);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    sensors_init();
    sensorData.id = 0;

    // Status LED Update
    led_setDelay(LED_ALWAYS_ON);
    int32_t ledDelay = LED_ALWAYS_ON;

    xTaskCreatePinnedToCore(taskGPS, "taskGPS", 4096, NULL, 1, &taskHandleGPS, 1);//Se crea una tarea de baja prioridad

    TickType_t xLastWakeTime = xTaskGetTickCount();

    i2cmessages_data supplyData;
    
    for(;;)
    {

        // Read Supply Board data
        i2cmessage_read(&supplyData);

        ESP_LOGE("MAIN", "Supply Data: ID: %d, Status: %d, V5: %d, VBatt: %d, I33: %d, I5: %d, Temp: %d, Config: %d, Internal Temp: %d",
                 supplyData.id,
                 supplyData.status,
                 supplyData.v5,
                 supplyData.vbatt,
                 supplyData.i33,
                 supplyData.i5,
                 supplyData.temp,
                 supplyData.config,
                 supplyData.internalTemp);

        sensorData.timestamp = esp_timer_get_time(); // Get timestamp in milliseconds
        sensorData.v5voltage = supplyData.v5 ; 
        sensorData.v3v3current = supplyData.i33 ; 
        sensorData.v5current = supplyData.i5 ;
        sensorData.vin = supplyData.vbatt;
        sensorData.supplyPcbTemp = supplyData.temp; 
        sensorData.supplyMCUTemp = supplyData.internalTemp; 
        sensorData.id++;


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
        if ( bz251Data.valid )
        {
            ledDelay = LED_ALWAYS_ON; // Keep LED on if valid GPS data
            led_setDelay(ledDelay); // Update LED delay
        }
        else
        {
            ledDelay = (ledDelay == LED_ALWAYS_ON) ? 0 : LED_ALWAYS_ON; // Toggle LED delay
            led_setDelay(ledDelay); // Update LED delay
        }
        
    }
}
