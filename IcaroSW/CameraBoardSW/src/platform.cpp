
#include "device/icam.h"
#include "device/ifilesystem.h"

#include "device/camera/ov2640.h"
#include "device/filesystem/sdcard.h"

#include "network/inetwork.h"
#include "network/wifi.h"
#include "network/wifiraw.h"
#include "network/link/ilink.h"

namespace 
{
static Network::WiFi* wifi = nullptr;
}

namespace Platform
{

Device::ICamera* buildCamera()
{
    return new Device::Camera::OV2640();
}

Device::IFileSystem* buildFileSystem()
{
    return new Device::FileSystem::SDCard();
}

Network::INetwork* buildWiFi()
{   
    if (wifi == nullptr)
    {
        wifi = new Network::WiFi();
    }
    return wifi;
}

Network::Link::ILink* buildUDPLink(char* ipDst, int port)
{
    Network::Link::ILink* link = nullptr;
    if (wifi != nullptr)
    {
        link = wifi->createUDPLink(ipDst, port);
    }
    return link;
}

Network::Link::ILink* buildTCPLink(char* ipDst, int port)
{
    Network::Link::ILink* link = nullptr;
    if (wifi != nullptr)
    {
        link = wifi->createTCPLink(ipDst, port);
    }
    return link;
}

}