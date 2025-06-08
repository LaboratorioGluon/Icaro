#include <bz251.h>
#include <bz251_defs.h>
#include <esp_log.h>
#include <driver/uart.h>
#include <sstream>
#include <string.h>

using namespace std;

static const char *TAG = "BZ251";

void Bz251::init(Bz251Config config)
{
    uartNum = config.uartNum;
    timeZone = config.timeZone;
    if(config.hasGps!=CFG_SIGNAL_GPS_ENA_DEFAULT)
    {
        configSetValue(CFG_RATE_TIMEREF, 0x04);         // Align measurements to Galileo time
        vTaskDelay(pdMS_TO_TICKS(500));
        configSetValue(CFG_NAVSPG_UTCSTANDARD, 0x05);   // UTC as combined from multiple European laboratories; derived from Galileo time
        vTaskDelay(pdMS_TO_TICKS(500));
        configSetValue(CFG_SIGNAL_SBAS_ENA,0);          // Disable SBAS for GPS disable
        vTaskDelay(pdMS_TO_TICKS(500));
        configSetValue(CFG_SIGNAL_QZSS_ENA,0);          // Disable QZSS for GPS disable
        vTaskDelay(pdMS_TO_TICKS(500));
        configSetValue(CFG_SIGNAL_GPS_ENA,0);           // GPS disable
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if(config.hasGalileo!=CFG_SIGNAL_GAL_ENA_DEFAULT)
    {
        configSetValue(CFG_SIGNAL_GAL_ENA,0); // Galileo disable
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if(config.hasBeidou!=CFG_SIGNAL_BDS_ENA_DEFAULT)
    {
        configSetValue(CFG_SIGNAL_BDS_ENA,1); // BeiDou enable
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    configSetValue(CFG_SIGNAL_BDS_ENA,0); // BeiDou enable
        vTaskDelay(pdMS_TO_TICKS(500));

    if(config.hasGlonass!=CFG_SIGNAL_GLO_ENA_DEFAULT)
    {
        configSetValue(CFG_SIGNAL_GLO_ENA,1); // Glonass enable
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if(config.dynmodel!=CFG_NAVSPG_DYNMODEL_DEFAULT)
    {
        configSetValue(CFG_NAVSPG_DYNMODEL,config.dynmodel);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    configSetValue(CFG_MSGOUT_NMEA_ID_GSA_UART1, 0);    // Disable nmea gsa
    configSetValue(CFG_MSGOUT_NMEA_ID_VTG_UART1, 0);    // Disable nmea vtg
    configSetValue(CFG_MSGOUT_NMEA_ID_GSV_UART1, 0);    // Disable nmea gsc
    configSetValue(CFG_MSGOUT_NMEA_ID_GLL_UART1, 0);    // Disable nmea gll
}
/* Initial configuration bz251*/

uint8_t Bz251::getData(Bz251Data &dev)
{
    getPosition(dev.latitude, dev.longitude);
    getAltitude(dev.altitude);
    getTime(dev.hour, dev.minute);
    getDate(dev.day, dev.month, dev.year);
    getSpeed(dev.speedKmh);
    getSatellites(dev.satellites);
    dev.valid =this->rmc.track_true; 

    return 0;
}
/* Set data in Bz251Data struct */


uint8_t Bz251::parse_NMEA(char *data)
{
    // Check header matching $xxRMC or $xxGGA
    if (data[0] != '$')
    {
        return 1;
    }

    ESP_LOGE("BZ251", "Parsing NMEA: %s", data);
    // Check if data is RMC or GGA
    if (data[3] == 'R' && data[4] == 'M' && data[5] == 'C')
    {
        return parse_RMC(data);
    }
    else if (data[3] == 'G' && data[4] == 'G' && data[5] == 'A')
    {
        return parse_GGA(data);
    }
    else
    {
        return 3;
    }
}

uint8_t Bz251::parse_GGA(char *data)
{
    // Check header matching $xxGGA
    if (data[0] != '$' || data[3] != 'G' || data[4] != 'G' || data[5] != 'A')
    {
        return 1;
    }

    // Split data into fields
    char *token = strtok(data, ",");
    uint8_t pos = 0;
    string GXGGA[15];
    while (token != NULL)
    {
        GXGGA[pos] = token;
        pos++;
        token = strtok(NULL, ",");
    }

    // Check number of expected fields is correct
    if (pos != 14)
    {
        return 2;
    }

    if (GXGGA[1]!=""){this->gga.utc = stoi(GXGGA[1].substr(0,6));}
    if (GXGGA[2]!=""){this->gga.latitude = stof(GXGGA[2])/100;}
    if (GXGGA[3]!=""){this->gga.lat_dir = GXGGA[3][0];}
    if (GXGGA[4]!=""){this->gga.longitude = stof(GXGGA[4])/1000;}
    if (GXGGA[5]!=""){this->gga.lon_dir = GXGGA[5][0];}
    if (GXGGA[6]!=""){this->gga.quality = GXGGA[6][0];}
    if (GXGGA[7]!=""){this->gga.satellites = stoi(GXGGA[7]);}
    if (GXGGA[8]!=""){this->gga.hdop = stof(GXGGA[8]);}
    if (GXGGA[9]!=""){this->gga.altitude = stof(GXGGA[9]);}
    if (GXGGA[10]!=""){this->gga.a_units = GXGGA[10][0];}
    if (GXGGA[11]!=""){this->gga.undulation = stof(GXGGA[11]);}
    if (GXGGA[12]!=""){this->gga.u_units = GXGGA[12][0];}

    return 0;
}


uint8_t Bz251::parse_RMC(char *data)
{
    // Check header matching $xxRMC
    if (data[0] != '$' || data[3] != 'R' || data[4] != 'M' || data[5] != 'C')
    {
        return 1;
    }

    // Split data into fields
    string GXRMC[13];
    
    // Fill GXRMC using strchr
    char *p2 = strchr(data, ',');
    char *p1 = data;
    int pos = 0;
    while (p2 != NULL)
    {
        GXRMC[pos] = string(p1, p2 - p1);
        p1 = p2 + 1;
        p2 = strchr(p1, ',');
        pos++;
    }

    // Check number of expected fields is correct
    if (pos != 13)
    {
        return 2;
    }

    // print GXRMC fields
    /*for (int i = 0; i < 13; i++)
    {
        ESP_LOGE("BZ251", "GXRMC[%d]: %s", i, GXRMC[i].c_str());
    }*/

    if (GXRMC[1]!=""){this->rmc.utc = stoi(GXRMC[1].substr(0,6));}
    if (GXRMC[2]!=""){this->rmc.pos_status = GXRMC[2][0];}
    if (GXRMC[3]!=""){this->rmc.latitude = stof(GXRMC[3])/100;}
    if (GXRMC[4]!=""){this->rmc.lat_dir = GXRMC[4][0];}
    if (GXRMC[5]!=""){this->rmc.longitude = stof(GXRMC[5])/1000;}
    if (GXRMC[6]!=""){this->rmc.lon_dir = GXRMC[6][0];}
    if (GXRMC[7]!=""){this->rmc.speed_Kn = stof(GXRMC[7]);}
    if (GXRMC[8]!=""){this->rmc.track_true = stof(GXRMC[8]);}
    if (GXRMC[9]!=""){this->rmc.date = stoi(GXRMC[9].substr(0,6));}
    if (GXRMC[10]!=""){this->rmc.mag_var = stof(GXRMC[10]);}
    if (GXRMC[11]!=""){this->rmc.var_dir = GXRMC[11][0];}
    if (GXRMC[12]!=""){this->rmc.mode_ind = GXRMC[12][0];}  

    return 0;
}

uint8_t Bz251::read(void)
{
    uint32_t len = uart_read_bytes(uartNum, &alldata[alldataLen], 512, 0);
    //ESP_LOGE("BZ251", "Read %lu bytes", len);
    while(len){
        alldataLen += len;
        len = uart_read_bytes(uartNum, &alldata[alldataLen], 512, 0);
    }
    
    // Find dolar sign and end
    uint32_t dFound=0, eFound=0, lasteFound = 0;
    for( uint32_t i = 0; i < alldataLen; i++)
    {
        if (alldata[i] == '$')
        {
            dFound = i;
        }
        if (alldata[i] == '\r')
        {
            eFound = i;
            lasteFound = eFound;
            if (dFound)
            {
                alldata[eFound] = 0;

                esp_err_t err = parse_NMEA((char*)&alldata[dFound]);

                dFound = 0;
                eFound = 0;
            }
        }
    }

    if (lasteFound)
    {
        alldataLen = alldataLen - lasteFound;
        for( uint32_t i = 0; i < alldataLen; i++)
        {
            alldata[i] = alldata[lasteFound + i];
        }
    }

    return 0;
}
/* Read the data received from the bz251 module */

uint8_t Bz251::getPosition(float &latitude, float &longitude)
{
    if( this->rmc.pos_status == 65)
    {
        latitude = this->rmc.latitude;
        longitude = this->rmc.longitude;
        if (this->rmc.lat_dir == 83){ latitude = latitude * -1; };
        if (this->rmc.lon_dir == 87){ longitude = longitude * -1; };
    }

    return 0;
}
/* Get latitude and longitude */

uint8_t Bz251::getAltitude(float &altitude)
{
    if( this->gga.satellites > 0)
    {
        altitude = this->gga.altitude;
    }

    return 0;
}
/* Antenna altitude above/below mean sea level */

uint8_t Bz251::getWeekday(uint8_t day, uint8_t month, uint8_t year)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        if( month < 3 )
        {
          year -= 1;
        }
        return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}
/* Get weekday, used for calculate winter/summer time change*/

uint8_t Bz251::getFirstDay(uint8_t year)
{
    for (uint8_t day=31; day>0; day--)
        {
            if (getWeekday(day, 3, year) == 0)
            {
                return day;
            }
        }
    return 0;
}
 /* Calculate last sunday of march, winter/summer time change*/

uint8_t Bz251::getLastDay(uint8_t year)
{
    for (uint8_t day=31; day>0; day--)
        {
            if (getWeekday(day, 10, year) == 0)
            {
                return day;
            }
        }
    return 0;
}
/* Calculate last sunday of october, winter/summer time change* */

uint8_t Bz251::addTimeZone(uint8_t &hour, uint8_t &day, uint8_t &month, uint8_t &year)
{
    uint8_t daysMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
    {
        daysMonth[1]=29;
    }

    hour++;
    if (hour > 23)
    {
        hour = 0;
        day++;
        if (day > daysMonth[month-1])
        {
            day = 1;
            month++;
            if (month > 12)
            {
                month = 1;
                year++;
            }
        }
    }
    return 0;
}
/* Update time with timezone */

uint8_t Bz251::sync(uint32_t &rawTime, uint32_t &rawDate)
{
    string strRawTime = to_string(rawTime);

    uint8_t hour = stoi(strRawTime.substr(0,2));
    uint8_t minute = stoi(strRawTime.substr(2,2));
    uint8_t second = stoi(strRawTime.substr(4,2));

    string strRawDate = to_string(rawDate);

    uint8_t day = stoi(strRawDate.substr(0,2));
    uint8_t month = stoi(strRawDate.substr(2,2));
    uint8_t year = stoi(strRawDate.substr(4,2));

    for (uint8_t i=0; i<this->timeZone; i++)
    {
        addTimeZone(hour, day, month, year);
    }

    if (((month > 3) && (month < 10)) || 
        ((month == 3) && (day >= getFirstDay(year))) || 
        ((month == 10) && (day <= getLastDay(year))))
    {
        addTimeZone(hour, day, month, year);
    }

    this->rmc.utc = (hour * 10000) + (minute * 100) + second;
    this->rmc.date = (day * 10000) + (month * 100) + year;

    return 0;
}
/* Update time and date with TimeZone */

uint8_t Bz251::getTime(uint8_t &hour, uint8_t &minute)
{
    if( this->rmc.pos_status == 65)
    {
        sync(this->rmc.utc, this->rmc.date);
        string strRawTime = to_string(this->rmc.utc);
        if (this->rmc.utc >= 100000)
        {
            hour = stoi(strRawTime.substr(0,2));
            minute = stoi(strRawTime.substr(2,2));
        }
        else
        {
            hour = strRawTime[0];
            minute = stoi(strRawTime.substr(1,2));
        }
    }
    return 0;
}
/* Get updated time with timezone*/

uint8_t Bz251::getDate(uint8_t &day, uint8_t &month, uint8_t &year)
{
    if( this->rmc.pos_status == 65)
    {
        sync(this->rmc.utc, this->rmc.date);
        string strRawDate = to_string(this->rmc.date);
        if (this->rmc.date >= 100000)
        {
            day = stoi(strRawDate.substr(0,2));
            month = stoi(strRawDate.substr(2,2));
            year = stoi(strRawDate.substr(4,2));
        }
        else
        {
            day = strRawDate[0];
            month = stoi(strRawDate.substr(1,2));
            year = stoi(strRawDate.substr(3,2));
        }
    }
    
    return 0;
}
/* Get updated date with timezone*/

uint8_t Bz251::getSpeed(float &speed)
{
    if( this->rmc.pos_status == 65)
    {
        speed = this->rmc.speed_Kn * 1.852001;

    }

    return 0;
}
/* Speed over ground, kmh */

uint8_t Bz251::getSatellites(uint8_t &satellites)
{
    if( this->gga.quality > 0)
    {
        satellites = this->gga.satellites;
    }

    return 0;
}
/* Number of satellites in use. May be different to the number in view */

uint8_t Bz251::setTimeZone(uint8_t tZone)
{
    this->timeZone = tZone;

    return 0;
}
/* Set TimeZona UTC */



/****************************
 *                          *
 *                          *
 *         CONFIG           *
 *                          *
 *                          *
 ***************************/

 void Bz251::calculateChecksum(uint8_t *message, uint8_t length) {
    uint8_t ck_a = 0, ck_b = 0;
    for (int i = 2; i < length - 2; i++) {
        ck_a += message[i];
        ck_b += ck_a;
    }
    message[length - 2] = ck_a;
    message[length - 1] = ck_b;
}

void Bz251::reset() {
    uint8_t ubx_reset_cmd[] = {
        0xB5, 0x62,  // Sync chars
        0x06, 0x04,  // Clase e ID
        0x04, 0x00,  // Longitud del payload
        0xFF, 0xFF, 0x00, 0x00,  // Payload: Cold start
        0x00, 0x00  // Checksum (placeholder)
    };

    calculateChecksum(ubx_reset_cmd, sizeof(ubx_reset_cmd));
    uart_write_bytes(uartNum, (const char*)ubx_reset_cmd, sizeof(ubx_reset_cmd));

    ESP_LOGE(TAG, "Restart");
}

void Bz251::factoryReset() {
    uint8_t ubx_cfg_cfg_factory_reset[] = {
        0xB5, 0x62,  // Encabezado UBX
        0x06, 0x09,  // Clase e ID (CFG-CFG)
        0x0D, 0x00,  // Longitud del payload (13 bytes)
        0xFF, 0xFF, 0xFF, 0xFF,  // clearMask: Borrar todo (0xFFFF)
        0x00, 0x00, 0x00, 0x00,  // saveMask: No guardar nada (0x0000)
        0xFF, 0xFF, 0xFF, 0xFF,  // loadMask: Cargar todo desde la memoria no volátil (0xFFFF)
        0x07,
        0x00, 0x00   // Checksum (placeholder)
    };

    calculateChecksum(ubx_cfg_cfg_factory_reset, sizeof(ubx_cfg_cfg_factory_reset));
    uart_write_bytes(uartNum, (const char*)ubx_cfg_cfg_factory_reset, sizeof(ubx_cfg_cfg_factory_reset));
}

void Bz251::configSetValue(uint64_t key, uint8_t value)
{
    uint8_t len = 0x09;
    uint8_t ubx_cfg[] = {
        0xB5, 0x62, // Sync chars (UBX header)
        0x06, 0x8A, // Clase e ID del mensaje
        len, 0x00, // Longitud del payload
        0x00, 0x01, 0x00, 0x00, // Message version(0), 1-ram 2-bbr 4-flash, reserved
        (uint8_t)(key), (uint8_t)(key >> 8), (uint8_t)(key >> 16), (uint8_t)(key >> 24),
        value,
        0x00, 0x00 // Checksum (placeholder)
    };

    calculateChecksum(ubx_cfg, sizeof(ubx_cfg));
    uart_write_bytes(uartNum, (const char*)ubx_cfg, sizeof(ubx_cfg));
}

void Bz251::configGetValue(uint64_t key)
{
    uint8_t ubx_cfg[] = {
        0xB5, 0x62, // Sync chars (UBX header)
        0x06, 0x8B, // Clase e ID del mensaje
        0x08, 0x00, // Longitud del payload
        0x00, 0x00, 0x00, 0x00, // Message version(0), 0-ram 1-bbr 2-flash 7-default, reserved
        (uint8_t)(key), (uint8_t)(key >> 8), (uint8_t)(key >> 16), (uint8_t)(key >> 24),
        0x00, 0x00 // Checksum (placeholder)
    };

    calculateChecksum(ubx_cfg, sizeof(ubx_cfg));
    uart_write_bytes(uartNum, (const char*)ubx_cfg, sizeof(ubx_cfg));
}

void Bz251::setValueFlash(uint64_t key, uint8_t value)
{
    uint8_t len = 0x09;
    uint8_t ubx_cfg[] = {
        0xB5, 0x62, // Sync chars (UBX header)
        0x06, 0x8A, // Clase e ID del mensaje
        len, 0x00, // Longitud del payload
        0x00, 0x07, 0x00, 0x00, // Message version(0), 1-ram 2-bbr 4-flash, reserved
        (uint8_t)(key), (uint8_t)(key >> 8), (uint8_t)(key >> 16), (uint8_t)(key >> 24),
        value,
        0x00, 0x00 // Checksum (placeholder)
    };

    calculateChecksum(ubx_cfg, sizeof(ubx_cfg));
    uart_write_bytes(uartNum, (const char*)ubx_cfg, sizeof(ubx_cfg));
}