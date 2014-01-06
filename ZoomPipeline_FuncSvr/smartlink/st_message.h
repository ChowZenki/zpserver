#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H

#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>
typedef struct tag_smartlink_message{
    unsigned char Mark[2];    //Always be "S" and "T" in upper case
    unsigned __INT32_TYPE__ version; //Structure Version
    unsigned __INT32_TYPE__ source_id ; //0xffffffff means from server
    unsigned __INT32_TYPE__ destin_id ; //0xffffffff means to server
    struct tag_payload{
        unsigned __INT32_TYPE__ data_length; //Data at least have 2 byte, for heart-beating counter.
        __UINT8_TYPE__ data[2];
    } payload;
   } SMARTLINK_MSG;
#endif

#if defined(_MSC_VER)
typedef struct tag_smartlink_message{
    unsigned char Mark[2];    //Always be "S" and "T" in upper case
    unsigned __int32 version; //Structure Version
    unsigned __int32 source_id ; //0xffffffff means from server
    unsigned __int32 destin_id ; //0xffffffff means to server
    struct tag_payload{
        unsigned __int32 data_length; //Data at least have 2 byte, for heart-beating counter.
        unsigned __int8 data[2];
    } payload;
   } SMARTLINK_MSG;
#endif



#pragma pack(pop)

#endif // ST_MESSAGE_H
