#ifndef ST_MSG_APPLAYER_H
#define ST_MSG_APPLAYER_H

namespace SmartLink{

#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>

//Host Reg request, send 64 byte Serial Num to Server
//SMARTLINK_MSG_APP::MsgType =  0x1000
typedef struct tag_stMsg_HostRegistReq{
    char HostSerialNum[64];
}stMsg_HostRegistReq;

//Host Reg replay, send back 4 byte ID
//SMARTLINK_MSG_APP::MsgType =  0x1800
typedef struct tag_stMsg_HostRegistRsp{
    __UINT8_TYPE__ DoneCode;//0 first ok, 1 ok 2 failed
    char TextInfo[64];
    __UINT32_TYPE__ ID;
}stMsg_HostRegistRsp;


typedef struct tag_smartlink_app_layer{
    struct tag_app_layer_header{
        __UINT16_TYPE__ MsgFmtVersion;
        __UINT8_TYPE__ AskID;
        __UINT16_TYPE__ MsgType;
    } header;
    union  union_MsgUnion
    {
         stMsg_HostRegistReq msg_HostRegistReq;
    }MsgUnion;
} SMARTLINK_MSG_APP;

#endif

#if defined(_MSC_VER)

//Host Reg request, send 64 byte Serial Num to Server
//SMARTLINK_MSG_APP::MsgType =  0x1000
typedef struct tag_stMsg_HostRegistReq{
    char HostSerialNum[64];
}stMsg_HostRegistReq;


//Host Reg replay, send back 4 byte ID
//SMARTLINK_MSG_APP::MsgType =  0x1800
typedef struct tag_stMsg_HostRegistRsp{
    unsigned __int8 DoneCode;
    char TextInfo[64];
    unsigned __int32 ID;
}stMsg_HostRegistRsp;


typedef struct tag_smartlink_app_layer{
    struct tag_app_layer_header{
        unsigned __int16 MsgFmtVersion;
        unsigned __int8 AskID;
        unsigned __int16 MsgType;
    } header;
    union  union_MsgUnion
    {
        stMsg_HostRegistReq msg_HostRegistReq;
    }MsgUnion;

} SMARTLINK_MSG_APP;
}
#endif



#pragma pack(pop)


#endif // ST_MSG_APPLAYER_H
