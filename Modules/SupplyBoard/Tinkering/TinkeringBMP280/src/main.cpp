#include <freertos/FreeRTOS.h>
#include <driver/i2c_master.h>

#include <string.h>

#include "sensorBme280.h"
#include <driver/i2c.h>
#include <esp_log.h>

#if 0
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define SDA_PIN 26
#define SCL_PIN 27

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
//Bme280 bme280;

bme280_dev dev;

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;
i2c_device_config_t dev_cfg;

BME280_INTF_RET_TYPE main_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    ESP_LOGE("MAIN", "TEST!");
    /*ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &reg_addr, 1, -1));
    ESP_ERROR_CHECK(i2c_master_receive(dev_handle, reg_data, len, -1));
    */
    ESP_ERROR_CHECK(
        i2c_master_transmit_receive(dev_handle, &reg_addr, 1, reg_data, len, -1)
        );
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

extern "C" void app_main() {

    dev.intf = BME280_I2C_INTF;
    dev.delay_us = main_bmi_delay;
    dev.read = main_i2c_read;
    dev.write = main_i2c_write;

    i2c_master_bus_config_t i2cMasterConfig = 
    {
        .i2c_port = I2C_NUM_0,                    /*!< I2C port number, `-1` for auto selecting, (not include LP I2C instance) */
        .sda_io_num = GPIO_NUM_26,                /*!< GPIO number of I2C SDA signal, pulled-up internally */
        .scl_io_num = GPIO_NUM_27,                /*!< GPIO number of I2C SCL signal, pulled-up internally */
        .clk_source = I2C_CLK_SRC_DEFAULT,            /*!< Clock source of I2C master bus */
        .glitch_ignore_cnt = 7U,                  /*!< If the glitch period on the line is less than this value, it can be filtered out, typically value is 7 (unit: I2C module clock cycle)*/
        .intr_priority = 0U,                      /*!< I2C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
        .trans_queue_depth = 100*10,                      /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
        .flags = {
            .enable_internal_pullup = 1
        }
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMasterConfig, &bus_handle));

    dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x76,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    ESP_LOGE("MAIN", "Init: %d", bme280_init(&dev));

    for(;;)
    {
        ESP_LOGE("MAIN", "Loop");
        ESP_LOGE("MAIN", "Init: %d", bme280_init(&dev));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif