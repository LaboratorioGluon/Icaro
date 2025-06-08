#include "Lora.h"
#include "si2c.h"
#include "sensors.h"

extern "C" void app_main(void);

Lora LoraModule;

bool datareceived = false;
uint8_t rx_buffer[128] = {0}; // Buffer to hold received data
uint16_t rx_length = 0; // Length of received data

SensorDataLora sensorDataLora;

bool i2c_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
  return true;
}

bool i2c_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    //printf("Received data length: %d\n", (int)evt_data->length);
    // Handle received data here
    // For example, you can log the received data
    if ( evt_data->length == sizeof(SensorDataLora)) {
      datareceived = true;
      
      memcpy(rx_buffer, evt_data->buffer, evt_data->length);
      rx_length = evt_data->length;
    }
    return true; // Return true to indicate that the event was handled
}


void debugSensorData(SensorDataLora &sensorData)
{
    // Print sensor data in differente lines:
    ESP_LOGE("SENSORS", "Timestamp: %lld", sensorData.timestamp);
    ESP_LOGE("SENSORS", "PT100: %.2f %.2f", sensorData.pt100_1, sensorData.pt100_2);
    ESP_LOGE("SENSORS", "BME280: %.2f %.2f %.2f",
             sensorData.bme.temperature,
             sensorData.bme.pressure,
             sensorData.bme.humidity);
    ESP_LOGE("SENSORS", "BMI160: %.2f %.2f %.2f %.2f %.2f %.2f", sensorData.acc.x, sensorData.acc.y, sensorData.acc.z, sensorData.gyr.x, sensorData.gyr.y, sensorData.gyr.z);
    ESP_LOGE("SENSORS", "BZ251: %f %f %f", sensorData.gps.latitude, sensorData.gps.longitude, sensorData.gps.altitude);
    ESP_LOGE("SENSORS", "BZ251 Time: %02d:%02d",
             sensorData.gps.hour,
             sensorData.gps.minute);
    ESP_LOGE("SENSORS", "BZ251 Date: %02d/%02d/%02d",
             sensorData.gps.day,
             sensorData.gps.month,
             sensorData.gps.year);
    ESP_LOGE("SENSORS", "BZ251 Satellites: %d | %d", sensorData.gps.satellites, sensorData.gps.valid);

}

void app_main(void) 
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  printf("Start\n");

  gpio_config_t config;
  config.mode = GPIO_MODE_OUTPUT;
  config.pin_bit_mask = (1 << GPIO_NUM_27); // Set GPIO 15 as output
  config.intr_type = GPIO_INTR_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.pull_up_en = GPIO_PULLUP_DISABLE;

  gpio_config(&config);
  gpio_set_level(GPIO_NUM_27, 1); // Set GPIO 15 high

  si2c_init();
  si2c_setCallbacks(i2c_receive_cb, i2c_request_cb);

  //Initialize radio
  LoraModule.ResetRadio();
  LoraModule.Init();

  //Start loop
  for(;;){
    for (int i=1; i <= LORA_REPEATS; i++)
    {
      //LoraModule.BuildMessage();
      LoraModule.SendMessage();
    }
    vTaskDelay(pdMS_TO_TICKS(LORA_LOOPTIME * 1000));
    gpio_set_level(GPIO_NUM_27, !gpio_get_level(GPIO_NUM_27)); // Set GPIO 15 low

    if (datareceived)
    {
      printf("Received data: ");
      memcpy(&sensorDataLora, rx_buffer, sizeof(SensorDataLora));
      memcpy(&LoraModule.sensorData, rx_buffer, sizeof(SensorDataLora));
      debugSensorData(sensorDataLora);

      /*for (uint16_t i = 0; i < rx_length; i++) {
        printf("%02X ", rx_buffer[i]);
      }
      printf("\n");*/
      datareceived = false; // Reset the flag
    }
  }
}