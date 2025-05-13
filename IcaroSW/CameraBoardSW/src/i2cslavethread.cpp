#include "i2cslavethread.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
#include "data/states.h"

namespace
{
const char*           MODULE_TAG       = "TH_I2CSLAVE";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_DEBUG;
}

void i2cSlaveThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<i2cSlaveThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>&   systemStatus = convertedArg->systemStatus;
    std::shared_ptr<InterBoards::I2CSlave>&        i2cSlave     = convertedArg->i2cSlave;
    std::shared_ptr<Data::i2cSlaveThreadStatus_t>& status       = convertedArg->threadStatus;

    constexpr size_t MAX_BUFFER_SIZE = 256;
    uint8_t i2cBuffer[MAX_BUFFER_SIZE];

    // 2 - Thread loop
    while (true)
    {
        if(systemStatus->i2cSlaveEnabled)
        {
            status->state = Data::ThreadState::RUNNING;

            // 2.1 Wait for I2C incoming data
            size_t lenRecv = i2cSlave->read(i2cBuffer, sizeof(i2cBuffer));
            
            // 2.2 TODO: Process received data
            printf("Datos recibidos: ");
            for (int i = 0; i < lenRecv; i++) {
                printf("%02X ", i2cBuffer[i]);
            }
            printf("\n");
        }
        else
        {
            status->state = Data::ThreadState::SLEEPING;

            // Sleep while thread is not enabled
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
        }
    }

    // 3 - Deinit thread
    status->state = Data::ThreadState::STOPPED;
}
