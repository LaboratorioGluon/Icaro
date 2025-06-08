#include "si2c.h"

static i2c_slave_dev_handle_t slave_handle;

void si2c_init()
{

    i2c_slave_config_t slave_config = {
        .i2c_port = I2C_NUM_0, // Use I2C port 0
        .sda_io_num = GPIO_NUM_21, // SDA pin
        .scl_io_num = GPIO_NUM_22, // SCL pin
        .clk_source = I2C_CLK_SRC_DEFAULT, // Default clock source
        .send_buf_depth = 128, // Depth of internal transfer ringbuffer
        .receive_buf_depth = 128, // Depth of receive internal software buffer
        .slave_addr = 0x28, // I2C address of the slave device
        .addr_bit_len = I2C_ADDR_BIT_LEN_7, // 7-bit address length
        .intr_priority = 1, // Interrupt priority
        .flags = {
            .allow_pd = 1, // Allow power domain management
            .enable_internal_pullup = 0, // Enable internal pull-ups
        }
    };

    ESP_ERROR_CHECK(i2c_new_slave_device(&slave_config, &slave_handle));
}


void si2c_setCallbacks(i2c_slave_received_callback_t receiveCallback, i2c_slave_request_callback_t requestCallback)
{
    i2c_slave_event_callbacks_t cbs = {
        .on_request = requestCallback, // Set the request callback
        .on_receive = receiveCallback, // Set the receive callback
    };

    // Register the callbacks with the I2C slave device
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL));
}