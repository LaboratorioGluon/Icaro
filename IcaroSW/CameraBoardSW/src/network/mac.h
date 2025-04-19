#pragma once 

#include <array>
#include <string>
#include <stdint.h>

namespace Network
{
constexpr int MAC_LENGTH = 6;
typedef std::array<uint8_t, MAC_LENGTH> mac_t;

constexpr Network::mac_t DEFAULT_MAC {0x02, 0x6A, 0x9C, 0x1F, 0x3B, 0xE8};
constexpr Network::mac_t BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

std::string mac_to_string(const uint8_t mac[6]);
}