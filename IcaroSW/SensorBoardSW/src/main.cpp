
#include <freertos/FreeRTOS.h>
#include <esp_log.h>

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
extern "C" {
    #include "ADS1115.h"
}


ads1115_t ads1115_cfg = {
    .reg_cfg =  ADS1115_CFG_MS_MODE_SS | ADS1115_CFG_LS_DR_128SPS | ADS1115_CFG_MS_MUX_DIFF_AIN0_AIN1 | ADS1115_CFG_MS_PGA_FSR_6_144V,
    .dev_addr = 0x48,
  };


Bmi160 bmi;
bme280_dev dev;


i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;
i2c_device_config_t dev_cfg;

i2c_master_bus_handle_t i2cExternalBusHandle;
i2c_master_dev_handle_t i2cSupplyDev;

Bz251 bz251;
Bz251Data bz251Data;

// PINOUT UART
#define UART_NUM UART_NUM_2
#define GPIO_UART_TX GPIO_NUM_17
#define GPIO_UART_RX GPIO_NUM_16

esp_err_t uart_init(void)
{
    uart_config_t uart_config;
    memset(&uart_config, 0, sizeof(uart_config_t));
        uart_config.baud_rate = 115200;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.source_clk = UART_SCLK_DEFAULT;
        

    // We won't use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 1024 * 2, 2048, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, GPIO_UART_TX, GPIO_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    return ESP_OK;
}

esp_err_t bz251_init(void)
{
    Bz251Config conf;
        conf.uartNum = UART_NUM_2;
        conf.timeZone = 1;  // Timezone UTC +1
        conf.hasGps = 0;    // Disable GPS
        conf.dynmodel = 6;  // Airborne with <1g acceleration

    bz251.init(conf);

    return ESP_OK;
}

BME280_INTF_RET_TYPE main_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    ESP_LOGE("MAIN", "TEST!");
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &reg_addr, 1, -1));
    ESP_ERROR_CHECK(i2c_master_receive(dev_handle, reg_data, len, -1));
    
    /*ESP_ERROR_CHECK(
        i2c_master_transmit_receive(dev_handle, &reg_addr, 1, reg_data, len, -1)
        );*/
    return BME280_INTF_RET_SUCCESS;

}

BME280_INTF_RET_TYPE main_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                                                    void *intf_ptr)
{
    uint8_t buf[50];
    buf[0] = reg_addr;
    memcpy(&buf[1], reg_data, len);


    esp_err_t err = i2c_master_transmit(dev_handle,
        buf,
        len+1,
        -1);

    if (err == ESP_OK)
    {
        return BME280_INTF_RET_SUCCESS;
    }
    return -1;
}

void main_bmi_delay(uint32_t period, void *intf_ptr)
{
    if (period < 10)
    {
        period = 10;
    }
    vTaskDelay(pdMS_TO_TICKS(period));
}

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
        //uart_flush(UART_NUM_2);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

extern "C" void app_main() {

    uart_init();
    bz251_init();
    i2c_master_bus_config_t i2cMasterConfig = 
    {
        .i2c_port = I2C_NUM_0,                    /*!< I2C port number, `-1` for auto selecting, (not include LP I2C instance) */
        .sda_io_num = GPIO_NUM_21,                /*!< GPIO number of I2C SDA signal, pulled-up internally */
        .scl_io_num = GPIO_NUM_22,                /*!< GPIO number of I2C SCL signal, pulled-up internally */
        .clk_source = I2C_CLK_SRC_DEFAULT,            /*!< Clock source of I2C master bus */
        .glitch_ignore_cnt = 7U,                  /*!< If the glitch period on the line is less than this value, it can be filtered out, typically value is 7 (unit: I2C module clock cycle)*/
        .intr_priority = 0U,                      /*!< I2C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
        .trans_queue_depth = 0,                      /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
        .flags = {
            .enable_internal_pullup = 1
        }
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMasterConfig, &bus_handle));

    i2cMasterConfig.i2c_port = I2C_NUM_1;
    i2cMasterConfig.sda_io_num = GPIO_NUM_25;
    i2cMasterConfig.scl_io_num = GPIO_NUM_26;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMasterConfig, &i2cExternalBusHandle));

    dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x48,
        .scl_speed_hz = 100000
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    dev_cfg.device_address = 0x58;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2cExternalBusHandle, &dev_cfg, &i2cSupplyDev));


    ESP_LOGE("MAIN", "Holi");
    ADS1115_initiate(&ads1115_cfg);
    
    //xTaskCreatePinnedToCore(coreAThread, "core_A", 4096, NULL, 3, &taskGPS, 0);

    esp_err_t result;

    uint32_t i2cRecv;
    uint16_t supplyTemp;

    uint8_t addr = 0x4;

    for(;;)
    {
        addr = DATAMAP_TEMP_OFFSET;
        esp_err_t err = i2c_master_transmit(i2cSupplyDev, &addr, 1, 1000);
        if (err == ESP_OK)
        {
            err = i2c_master_receive(i2cSupplyDev, (uint8_t*)&supplyTemp, 2, 1000);
            ESP_LOGE("MAIN", "Temperatura supply: %d mV", ((supplyTemp&0xFF)<<8) + (supplyTemp>>8) );

        }
        else
        {
            ESP_LOGE("MAIN", "I2C Tx Error: %d", err);
        }
        /*
        esp_err_t err = i2c_master_transmit(i2cSupplyDev, &addr, 1, 1000);
        if (err == ESP_OK)
        {
            err = i2c_master_receive(i2cSupplyDev, (uint8_t*)&i2cRecv, 4, 1000);
            if (err == ESP_OK)
            {
                addr++;
                if (addr > 10)
                    addr = 0;
                ESP_LOGE("MAIN", "I2C Recv(%d): 0x%08lX", addr, i2cRecv);
            }
            else
            {
                ESP_LOGE("MAIN", "I2C Recv Error: %d", err);
            }

        }
        else
        {
            ESP_LOGE("MAIN", "I2C Tx Error: %d", err);
            //i2c_master_bus_reset(i2cSupplyDev);
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    bmi.init({SPI2_HOST, GPIO_NUM_23, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, 1000000});
    bmi.calibrate(5000);
    Bmi160::Data acc, gyr;

    vTaskDelay(pdMS_TO_TICKS(2000));


    /*i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);*/

    dev.intf = BME280_I2C_INTF;
    dev.delay_us = main_bmi_delay;
    dev.read = main_i2c_read;
    dev.write = main_i2c_write;


    ESP_LOGE("MAIN", "Init: %d", bme280_init(&dev));

    bme280_settings settings;

    bme280_get_sensor_settings(&settings, &dev);

    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_16X;
    settings.osr_t = BME280_OVERSAMPLING_2X;
    settings.filter = BME280_FILTER_COEFF_16;
    settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

    bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &dev);
    
    bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);

    ESP_LOGE("MAIN", "Get sensor settings: %d", bme280_get_sensor_settings(&settings, &dev));

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

}

#endif
