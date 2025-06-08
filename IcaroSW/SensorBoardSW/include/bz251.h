#ifndef BZ251_H__
#define BZ251_H__

#include <bz251_defs.h>
#include <stdint.h>
#include <driver/gpio.h>
#include <driver/uart.h>


typedef struct {
    uart_port_t uartNum;    /* UART port number, can be UART_NUM_0 ~ (UART_NUM_MAX -1) */
    uint8_t hasGps = CFG_SIGNAL_GPS_ENA_DEFAULT;         /* 1 Enable - 0 Disable */
    uint8_t hasGlonass = CFG_SIGNAL_GLO_ENA_DEFAULT;     /* 1 Enable - 0 Disable */
    uint8_t hasGalileo = CFG_SIGNAL_GAL_ENA_DEFAULT;     /* 1 Enable - 0 Disable */
    uint8_t hasBeidou = CFG_SIGNAL_BDS_ENA_DEFAULT;      /* 1 Enable - 0 Disable */
    uint8_t dynmodel;       /** 0 Portable
                                2 Stationary
                                3 Pedestrian
                                4 Automotive
                                5 Sea
                                6 Airborne with <1g acceleration
                                7 Airborne with <2g acceleration
                                8 Airborne with <4g acceleration
                                9 Wrist-worn watch (not available in all products)
                                10 Motorbike (not available in all products)
                                11 Robotic lawn mower (not available in all products)
                            */
    uint8_t timeZone;       /* UTC */
} Bz251Config;

typedef struct {
    uint8_t valid;
    uint8_t year;       /* yy */
    uint8_t month;      /* mm */
    uint8_t day;        /* dd */
    float latitude;     /* dd.mmmmmm */
    float longitude;    /* dd.mmmmmm */
    float altitude;     /* Antenna altitude above/below mean sea level (meters) */
    uint8_t hour;       /* hh */
    uint8_t minute;     /* mm */
    uint8_t satellites; /* Number of satellites in use. May be different to the number in view */
    uint8_t dummy;
    float speedKmh;     /* Speed over ground, kms */
} Bz251Data;

class Bz251
{
    public:

        void init(Bz251Config config);

        uint8_t getData(Bz251Data &dev);

        uint8_t read(void);
        uint8_t getPosition(float &latitude, float &longitude);
        uint8_t getAltitude(float &altitude);
        uint8_t getTime(uint8_t &hour, uint8_t &minute);
        uint8_t getDate(uint8_t &day, uint8_t &month, uint8_t &year);
        uint8_t getSpeed(float &speed);
        uint8_t getSatellites(uint8_t &satellites);
        uint8_t setTimeZone(uint8_t tZone);
    
        void configSetValue(uint64_t key, uint8_t value);
        void configGetValue(uint64_t key);
        void reset(void);
        void factoryReset(void);
        void setValueFlash(uint64_t key, uint8_t value);

    private:

        struct rmcData{
            uint32_t utc;           /* hhmmss.sss */
            uint8_t pos_status;     /* A=data valid or V=data not valid */
            float latitude;         /* ddmm.mmmm */
            uint8_t lat_dir;        /* N=north or S=south */
            float longitude;        /* dddmm.mmmm */
            uint8_t lon_dir;        /* E=east or W=west */
            float speed_Kn;         /* Speed over ground, knots */
            float track_true;       /* Track made good, degrees True */
            uint32_t date;          /* ddmmyy */
            float mag_var;          /* Magnetic variation, degrees */
            uint8_t var_dir;        /* degrees E=east or W=west */    
            uint8_t mode_ind;       /* A=Autonomous, D=Diferential, E=Estimated M=Manual, N=Data no valid */
        };
        rmcData rmc;

        struct ggaData{
            uint32_t utc;           /* hhmmss.sss */
            float latitude;         /* ddmm.mmmm */
            uint8_t lat_dir;        /* N=north or S=south */
            float longitude;        /* dddmm.mmmm */
            uint8_t lon_dir;        /* E=east or W=west */
            uint8_t quality;        /*  0 - Fix not available or invalid
                                        1 - Single point
                                            Converging PPP (TerraStar-L)
                                        2 - Pseudorange differential
                                            Converged PPP (TerraStar-L)
                                            Converging PPP (TerraStar-C PRO, TerraStar-X)
                                        4 - RTK fixed ambiguity solution
                                        5 - RTK floating ambiguity solution 
                                            Converged PPP (TerraStar-C PRO, TerraStar-X)
                                        6 - Dead reckoning mode
                                        7 - Manual input mode (fixed position)
                                        8 - Simulator mode
                                        9 - WAAS (SBAS)1 */
            uint8_t satellites;     /* Number of satellites in use. May be different to the number in view */
            float hdop;             /* Horizontal dilution of precision */
            float altitude;         /* Antenna altitude above/below mean sea level */
            uint8_t a_units;        /* Units of antenna altitude (M = metres) */       
            float undulation;       /* Undulation - the relationship between the geoid and the WGS84 ellipsoid */
            uint8_t u_units;        /* Units of undulation (M = metres) */
            uint8_t age;            /* Age of correction data (in seconds) */
            uint16_t stn_ID;        /* Differential base station ID */
        };
        ggaData gga;
        
        uint8_t getWeekday(uint8_t day, uint8_t month, uint8_t year);
        uint8_t getFirstDay(uint8_t year);
        uint8_t getLastDay(uint8_t year);
        uint8_t addTimeZone(uint8_t &hour, uint8_t &day, uint8_t &month, uint8_t &year);
        uint8_t sync(uint32_t &rawTime, uint32_t &rawDate); 

        uart_port_t uartNum;
        uint8_t timeZone = 1;

        void calculateChecksum(uint8_t *message, uint8_t length);

        uint8_t parse_NMEA(char *data);
        uint8_t parse_GGA(char *data);
        uint8_t parse_RMC(char *data);

        uint8_t alldata[2512];
        uint32_t alldataLen = 0;
        
};


#endif