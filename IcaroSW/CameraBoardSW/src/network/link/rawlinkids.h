#pragma once

namespace Network::Link
{
    enum class RAW_LINK_ID
    {
        TEST       = 0,
        RAW_IMAGE  = 1,
        MAV_STATUS = 2,
        MAV_IMAGE  = 3,
        // Link ID has 4 bytes, max value is 15/0xF
        LAST_VALID = 0xF
    };
}
    