
#include "system.h"

#include <freertos/FreeRTOS.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>

#include <string.h>
#include "led.h"
#include "i2cMessages.h"


i2c_master_bus_handle_t i2c_busInternal;
i2c_master_dev_handle_t i2cDeviceBme280;
i2c_master_dev_handle_t i2cDeviceAds1115;
i2c_master_dev_handle_t i2cDeviceBmi160;
i2c_device_config_t dev_cfg;

i2c_master_bus_handle_t i2c_busExternal;
i2c_master_dev_handle_t i2cSupplyDev;
i2c_master_dev_handle_t i2cCommsDev;



/*** UART ***/
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


/*** I2C ***/

#define I2C_ADS1117_ADDR 0x48
#define I2C_BME280_ADDR 0x76
#define I2C_BMI160_ADDR 0x68



void i2c_init()
{
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

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMasterConfig, &i2c_busInternal));

    i2cMasterConfig.i2c_port = I2C_NUM_1;
    i2cMasterConfig.sda_io_num = GPIO_NUM_25;
    i2cMasterConfig.scl_io_num = GPIO_NUM_26;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMasterConfig, &i2c_busExternal));

    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.scl_speed_hz = 100000;
    dev_cfg.device_address = I2C_ADS1117_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_busInternal, &dev_cfg, &i2cDeviceAds1115));

    dev_cfg.device_address = I2C_BME280_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_busInternal, &dev_cfg, &i2cDeviceBme280));

    dev_cfg.device_address = I2C_BMI160_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_busInternal, &dev_cfg, &i2cDeviceBmi160));

    dev_cfg.device_address = 0x58;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_busExternal, &dev_cfg, &i2cSupplyDev));

    dev_cfg.device_address = 0x28;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_busExternal, &dev_cfg, &i2cCommsDev));
}

void system_init()
{
    led_init();
    led_setDelay(250);
    
    uart_init();
    i2c_init();
    i2cmessages_init(i2cSupplyDev);
    i2cmessage_set5v(1); // Enable 5V supply by default

}