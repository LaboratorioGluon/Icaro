#include "mac.h"

namespace Network
{

std::string mac_to_string(const uint8_t mac[6]) 
{
    char str[18];
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(str);
}

}