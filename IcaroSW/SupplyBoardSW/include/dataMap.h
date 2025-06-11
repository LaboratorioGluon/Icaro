#ifndef __I2C_DATA_H
#define __I2C_DATA_H

#include <stdint.h>

#define DATAMAP_ID_OFFSET 0x0
#define DATAMAP_STATUS_OFFSET 0x1
#define DATAMAP_V5_OFFSET 0x2
#define DATAMAP_VBATT_OFFSET 0x4
#define DATAMAP_I33_OFFSET 0x6
#define DATAMAP_I5_OFFSET 0x8
#define DATAMAP_TEMP_OFFSET 0xA
#define DATAMAP_CONFIG_OFFSET 0xC
#define DATAMAP_INTERNAL_TEMP_OFFSET 0x10

#define DATAMAP_SIZE 0x20

uint8_t dataMap[DATAMAP_SIZE] = {0x01, 0,0,0,0,0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,0,0,0,0,
                                  0,0,0,0};



#endif //__I2C_DATA_H