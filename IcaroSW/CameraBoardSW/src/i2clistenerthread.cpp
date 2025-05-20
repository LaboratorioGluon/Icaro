#include "i2clistenerthread.h"

#include <time.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "systemdef.h"
#include "data/states.h"
#include "data/externalstatus.h"

#include "interboards/messages/ims_messages.h"

namespace
{
const char*           MODULE_TAG       = "TH_I2CLISTENER";
const esp_log_level_t MODULE_LOG_LEVEL = ESP_LOG_NONE;

void copyBoardStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::CameraMessages;
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
    using namespace InterBoards::CameraMessages;
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
    using namespace InterBoards::CameraMessages;
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
    using namespace InterBoards::CameraMessages;
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
    using namespace InterBoards::CameraMessages;
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
    using namespace InterBoards::CameraMessages;
    auto ims_gps = reinterpret_cast<const ImsMessageGPS*>(raw);
    externalStatus->gps = {
        .externalTS = ims_gps->header.timestamp,
        .latitude   = ims_gps->msg.latitude,
        .longitude  = ims_gps->msg.longitude,
        .altitude   = ims_gps->msg.altitude,
    };
}

void copyDateTimeStatus(const ImsMessageRaw* raw, std::shared_ptr<Data::ExternalStatus_t>& externalStatus)
{
    using namespace InterBoards::CameraMessages;
    auto ims_gps = reinterpret_cast<const ImsMessageDateTime*>(raw);
    externalStatus->time = {
        .boardTS     = static_cast<uint64_t>(time(NULL)),
        .externalTS  = ims_gps->header.timestamp,
        .year        = ims_gps->msg.year,
        .month       = ims_gps->msg.month,
        .day         = ims_gps->msg.day,
        .hour        = ims_gps->msg.hour,
        .minute      = ims_gps->msg.minute,
        .second      = ims_gps->msg.second,
        .millisecond = ims_gps->msg.millisecond,
    };
}

void updateSystemTime(const Data::ExternalTimeStatus_t& current)
{
    struct tm tm = {
        .tm_sec  = current.second,
        .tm_min  = current.minute,
        .tm_hour = current.hour,
        .tm_mday = current.day,
        .tm_mon  = current.month - 1,
        .tm_year = current.year - 1900,
        .tm_isdst = 0,
    };

    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
}

}

void i2cListenerThreadFunc (void* arg)
{
    esp_log_level_set(MODULE_TAG, MODULE_LOG_LEVEL);    
    ESP_LOGI(MODULE_TAG, "Thread launched");
    ESP_LOGI(MODULE_TAG, "Running in core %d", xPortGetCoreID());

    // 1 - Init thread
    // 1.1 - Parse arguments
    auto convertedArg = reinterpret_cast<i2cListenerThreadArg_t*>(arg);
    std::shared_ptr<const Data::systemStatus_t>&         systemStatus   = convertedArg->systemStatus;
    std::shared_ptr<Network::WiFiRaw>&                   wifi           = convertedArg->wifiraw;
    std::shared_ptr<InterBoards::I2CSlave>&              i2cSlave       = convertedArg->i2cSlave;
    std::shared_ptr<Data::i2cListenerThreadStatus_t>&    status         = convertedArg->threadStatus;
    std::shared_ptr<Data::ExternalStatus_t>&             externalStatus = convertedArg->externalStatus;

    constexpr size_t IMS_MESSAGE_SIZE = sizeof(ImsMessageRaw);
    uint8_t i2cBuffer[IMS_MESSAGE_SIZE];
    ImsMessageRaw* raw = reinterpret_cast<ImsMessageRaw*>(i2cBuffer);

    // 2 - Thread loop
    while (true)
    {
        ESP_LOGV(MODULE_TAG, "Cycle start");
        if(systemStatus->i2cListenEnabled)
        {
            status->state = Data::ThreadState::RUNNING;

            // 2.1 Wait for I2C incoming data
            size_t lenRecv = i2cSlave->read(i2cBuffer, sizeof(raw->header));
            
            if (lenRecv == sizeof(raw->header) && raw->header.len <= sizeof(raw->raw))
            {
                lenRecv = lenRecv + i2cSlave->read(raw->raw, raw->header.len);
            }
            else
            {
                // Not received a full header, skip for next cycle until bus is stabilized.
                continue;
            }

            // 2.2 TODO: Process received data
            // printf("Datos recibidos: ");
            // for (int i = 0; i < lenRecv; i++) {
            //     printf("%02X ", i2cBuffer[i]);
            // }
            // printf("\n");
            
            const size_t expectedSize = sizeof(raw->header) + raw->header.len;
            if (lenRecv == expectedSize)
            {
                using namespace InterBoards::CameraMessages;
                
                switch (static_cast<IMS_TAGS>(raw->header.tag))
                {
                case IMS_TAGS::Raw:
                    break;
                case IMS_TAGS::Status:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Status");
                    copyBoardStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "Status: { ts: %llu, sensorboard: %u, commsboard: %u, powerboard: %u }", 
                        externalStatus->boards.externalTS, externalStatus->boards.sensorBoardStatus, 
                        externalStatus->boards.commsBoardStatus, externalStatus->boards.powerBoardStatus);
                    break;
                case IMS_TAGS::Power:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Power");
                    copyPowerStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "Power: { ts: %llu, status3v3: %u, status5v: %u, batteryLevel: %f }", 
                        externalStatus->power.externalTS, externalStatus->power.status3v3, 
                        externalStatus->power.status5v, externalStatus->power.batteryLevel);
                    break;
                case IMS_TAGS::Sensors:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: Sensors");
                    copySensorsStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "Sensors: { ts: %llu, ExternalTemp: %f, InternalTemp: %f, OnboardTemp: %f, Humidity: %f}", 
                        externalStatus->sensors.externalTS, externalStatus->sensors.ExternalTemp, 
                        externalStatus->sensors.InternalTemp, externalStatus->sensors.OnboardTemp, 
                        externalStatus->sensors.Humidity);
                    break;
                case IMS_TAGS::AttitudeAccel:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: AttitudeAccel");
                    copyAccelStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "AttitudeAccel: { ts: %llu, X: %f, Y: %f, Z: %f}", 
                        externalStatus->accel.externalTS, externalStatus->accel.accelX, 
                        externalStatus->accel.accelY, externalStatus->accel.accelZ);
                    break;
                case IMS_TAGS::AttitudeGyro:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: AttitudeGyro");
                    copyGyroStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "AttitudeGyro: { ts: %llu, X: %f, Y: %f, Z: %f}", 
                        externalStatus->gyro.externalTS, externalStatus->gyro.gyroX, 
                        externalStatus->gyro.gyroY, externalStatus->gyro.gyroZ);
                    break;
                case IMS_TAGS::GPS:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: GPS");
                    copyGPSStatus(raw, externalStatus);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "GPS: { ts: %llu, lat: %f, lon: %f, alt: %f}", 
                        externalStatus->gps.externalTS, externalStatus->gps.latitude, 
                        externalStatus->gps.longitude, externalStatus->gps.altitude);
                    break;
                case IMS_TAGS::DateTime:
                    ESP_LOGI(MODULE_TAG, "Received IMS message: DateTime");
                    copyDateTimeStatus(raw, externalStatus);
                    updateSystemTime(externalStatus->time);
                    ESP_LOGD(MODULE_TAG, "TAG    = %u", raw->header.tag);
                    ESP_LOGD(MODULE_TAG, "Source = %u", raw->header.source);
                    ESP_LOGD(MODULE_TAG, "Length = %u", raw->header.len);
                    ESP_LOGD(MODULE_TAG, "Dummy  = %u", raw->header.dummy);
                    ESP_LOGD(MODULE_TAG, "DateTime: { ts: %llu, date: %04u/%02u/%02u, time: %02u:%02u:%02u.%03u}", 
                        externalStatus->time.externalTS, externalStatus->time.year, externalStatus->time.month, 
                        externalStatus->time.day, externalStatus->time.hour, externalStatus->time.minute, 
                        externalStatus->time.second, externalStatus->time.millisecond);
                    break;
                default:
                    ESP_LOGW(MODULE_TAG, "Received IMS message: Unknown TAG (%d)", raw->header.tag);
                    break;
                }
            }
        }
        else
        {
            ESP_LOGV(MODULE_TAG, "Sleeping");
            status->state = Data::ThreadState::SLEEPING;

            // Sleep while thread is not enabled
            static TickType_t lastWakeUpTime = 0;
            xTaskDelayUntil(&lastWakeUpTime, pdMS_TO_TICKS(500));
        }
    }

    // 3 - Deinit thread
    ESP_LOGE(MODULE_TAG, "Fatal error: Thread stopped");
    status->state = Data::ThreadState::STOPPED;
}
