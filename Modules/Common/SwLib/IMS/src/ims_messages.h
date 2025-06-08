#ifndef IMS_MESSAGES_H__
#define IMS_MESSAGES_H__

#include <stdint.h>

/** IMS messages **/

#define IMS_MESSAGE_MAXSIZE 50
#define IMS_PACK __attribute__((packed))


template<typename T>
struct IMS_PACK ImsMessageBase{

    /** Common header */
    struct IMS_PACK {
        uint64_t timestamp;
        uint8_t tag;
        uint8_t source;
        uint8_t len;
        uint8_t dummy;
    } header;
    static_assert(sizeof(header) ==  12);

    /** Message data */
    union 
    {
        uint8_t raw[IMS_MESSAGE_MAXSIZE];
        T msg;
    };
    static_assert(sizeof(T) <= sizeof(raw));
    
};

typedef ImsMessageBase<uint8_t[IMS_MESSAGE_MAXSIZE]> ImsMessageRaw;


/**
 * 
 */



#endif //IMS_MESSAGES_H__
