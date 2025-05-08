#include "mav_camera.h"

#include <freertos/FreeRTOS.h>

#include <common/mavlink_msg_camera_image_captured.h>

namespace MAVLink
{

MAVCamera::MAVCamera(uint8_t systemID,
                     uint8_t componentID,
                     std::unique_ptr<Network::Link::ILink> link) : 
    MAVComponent(std::move(link)), systemID(systemID), componentID(componentID)
{}

bool MAVCamera::notifyCapture(int32_t index, int8_t result, const char* filename,
                              int32_t lat, int32_t lon, int32_t alt)
{
    mavlink_message_t mavMessage;

    const int32_t timeSinceBoot = xTaskGetTickCount();
    const uint8_t cameraID = componentID - MAV_COMP_ID_CAMERA;
    const float q[4] = {0.0f};
    size_t message_length = mavlink_msg_camera_image_captured_pack(
        systemID, componentID, 
        &mavMessage, 
        pdTICKS_TO_MS(timeSinceBoot),
        0,
        cameraID,
        lat,
        lon,
        alt,
        -1,
        q,
        index,
        result,
        filename
    );
    bool ok = (send(mavMessage) == message_length);
    return ok;
}

}
