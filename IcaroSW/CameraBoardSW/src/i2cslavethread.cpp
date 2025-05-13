#include "i2cslavethread.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
#include "data/states.h"
#include "data/externalstatus.h"

#include "interboards/messages/ims_messages.h"

namespace
{
const char*           MODULE_TAG       = "TH_I2CSLAVE";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_DEBUG;

void copyBoardStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_status = reinterpret_cast<const ImsMessageStatus*>(raw);
    externalStatus->boards = {
        .externalTS        = ims_status->header.timestamp,
        .sensorBoardStatus = ims_status->msg.sensorBoardStatus,
        .commsBoardStatus  = ims_status->msg.commsBoardStatus,
        .powerBoardStatus  = ims_status->msg.powerBoardStatus,
    };
}

void copyPowerStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_power = reinterpret_cast<const ImsMessagePower*>(raw);
    externalStatus->power = {
        .externalTS   = ims_power->header.timestamp,
        .status3v3    = ims_power->msg.status3v3,
        .status5v     = ims_power->msg.status5v,
        .batteryLevel = ims_power->msg.batteryLevel,
    };
}

void copySensorsStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_sensors = reinterpret_cast<const ImsMessageSensors*>(raw);
    externalStatus->sensors = {
        .externalTS   = ims_sensors->header.timestamp,
        .InternalTemp = ims_sensors->msg.InternalTemp,
        .ExternalTemp = ims_sensors->msg.ExternalTemp,
        .OnboardTemp  = ims_sensors->msg.OnboardTemp,
        .Humidity     = ims_sensors->msg.Humidity,
    };
}

void copyAccelStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_accel = reinterpret_cast<const ImsMessageAccel*>(raw);
    externalStatus->accel = {
        .externalTS = ims_accel->header.timestamp,
        .accelX     = ims_accel->msg.accelX,
        .accelY     = ims_accel->msg.accelY,
        .accelZ     = ims_accel->msg.accelZ,
    };
}

void copyGyroStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_gyro = reinterpret_cast<const ImsMessageGyro*>(raw);
    externalStatus->gyro = {
        .externalTS = ims_gyro->header.timestamp,
        .gyroX      = ims_gyro->msg.gyroX,
        .gyroY      = ims_gyro->msg.gyroY,
        .gyroZ      = ims_gyro->msg.gyroZ,
    };
}

void copyGPSStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::Messages;
    auto ims_gps = reinterpret_cast<const ImsMessageGPS*>(raw);
    externalStatus->gps = {
        .externalTS = ims_gps->header.timestamp,
        .latitude   = ims_gps->msg.latitude,
        .longitude  = ims_gps->msg.longitude,
        .altitude   = ims_gps->msg.altitude,
    };
}

}

void i2cSlaveThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<i2cSlaveThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>&         systemStatus   = convertedArg->systemStatus;
    std::shared_ptr<Network::WiFiRaw>&                   wifi           = convertedArg->wifiraw;
    std::shared_ptr<InterBoards::I2CSlave>&              i2cSlave       = convertedArg->i2cSlave;
    std::shared_ptr<Data::i2cSlaveThreadStatus_t>&       status         = convertedArg->threadStatus;
    std::shared_ptr<Data::ExternalStatus_t>&              externalStatus = convertedArg->externalStatus;

    constexpr size_t IMS_MESSAGE_SIZE = sizeof(ImsMessageRaw);
    uint8_t i2cBuffer[IMS_MESSAGE_SIZE];
    const ImsMessageRaw* raw = reinterpret_cast<ImsMessageRaw*>(i2cBuffer);

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
            
            if (lenRecv == sizeof(ImsMessageRaw))
            {
                using namespace InterBoards::Messages;
                
                switch (static_cast<IMS_TAGS>(raw->header.tag))
                {
                case IMS_TAGS::Raw:
                    break;
                case IMS_TAGS::Status:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Status");
                    copyBoardStatus(raw, externalStatus);
                    break;
                case IMS_TAGS::Power:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Power");
                    copyPowerStatus(raw, externalStatus);
                    break;
                case IMS_TAGS::Sensors:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Sensors");
                    copySensorsStatus(raw, externalStatus);
                    break;
                case IMS_TAGS::AttitudeAccel:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: AttitudeAccel");
                    copyAccelStatus(raw, externalStatus);
                    break;
                case IMS_TAGS::AttitudeGyro:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: AttitudeGyro");
                    copyGyroStatus(raw, externalStatus);
                    break;
                case IMS_TAGS::GPS:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: GPS");
                    copyGPSStatus(raw, externalStatus);
                    break;
                default:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Unknown TAG (%d)", raw->header.tag);
                    break;
                }
            }
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
