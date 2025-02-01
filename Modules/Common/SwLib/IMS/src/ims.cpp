#include "ims.h"

Ims::Ims():iface(nullptr)
{

}

void Ims::init(ImsInterface &interface)
{
    iface = &interface;
    iface->init();
}


uint8_t Ims::_send(const uint8_t dst, const uint8_t cmd, const uint8_t * data, const uint8_t len)
{
    iface->send(dst, cmd, data, len);
    return 0;
}

uint8_t Ims::_recv(uint8_t &src, const uint8_t cmd, uint8_t* data, uint8_t len)
{
    iface->read(src, cmd, *data, len);
    return 0;
}

