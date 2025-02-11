#ifndef IMS_H__
#define IMS_H__

#include <stdio.h>
#include <stdint.h>
#include "ims_interface.h"
#include "ims_messages.h"
#include "HAL/ims_hal.h"

#include <string.h>

constexpr uint32_t IMS_BUFFER = 50;

class Ims{
public:
    Ims();

    void init(ImsInterface &interface);

    template<typename T>
    uint8_t send(const uint8_t dst, const uint8_t cmd, const T &msg);

    template<typename T>
    uint8_t receive(uint8_t &src, const uint8_t cmd, T& msg);


private:

    uint8_t _send(const uint8_t dst, const uint8_t cmd, const uint8_t * data, const uint8_t len);

    uint8_t _recv(uint8_t &src, const uint8_t cmd, uint8_t* data, uint8_t len);

    // Temporal buffer used to send and receive
    uint8_t buffer[IMS_BUFFER];

    ImsInterface *iface;
};

template<typename T>
uint8_t Ims::send(const uint8_t dst, const uint8_t cmd, const T &msg)
{
    ImsMessageBase<T> fullMsg ={0};
    fullMsg.header.timestamp = getTimeUs();
    fullMsg.msg = msg;
    fullMsg.header.len = sizeof(T);

    return _send(dst, cmd, (uint8_t*)&fullMsg.msg, fullMsg.header.len);
}

template<typename T>
uint8_t Ims::receive(uint8_t &src, const uint8_t cmd, T& msg)
{
    uint8_t ret = _recv(src, cmd, buffer, sizeof(T));
    memcpy((uint8_t*)&msg, buffer, sizeof(T));
    return ret;
}

#endif //IMS_H__