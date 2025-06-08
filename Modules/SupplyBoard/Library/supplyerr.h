#ifndef SUPPLYERR_H__
#define SUPPLYERR_H__

#include <stdint.h>

typedef enum
{
    SUPPLY_OK = 0,
    SUPPLY_ERR = -1,
    SUPPLY_TIMEOUT = -2,
    SUPPLY_BUSY = -3,
    SUPPLY_NOT_SUPPORTED = -4,
} SupplyErr;


#endif //SUPPLYERR_H__