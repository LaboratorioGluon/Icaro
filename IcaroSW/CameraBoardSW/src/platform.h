
#include "device/icam.h"
#include "device/ifilesystem.h"
#include "network/inetwork.h"
#include "network/link/ilink.h"

namespace Platform
{

Device::ICamera* buildCamera();

Device::IFileSystem* buildFileSystem();

Network::INetwork* buildWiFi();
Network::Link::ILink* buildUDPLink(char* ipDst, int port);
Network::Link::ILink* buildTCPLink(char* ipDst, int port);

}