#ifndef CROSS_SVR_MESSAGES_H
#define CROSS_SVR_MESSAGES_H

namespace ZP_Cluster{
#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>
	typedef struct tag_cross_svr_message{
		struct tag_header{
			__UINT16_TYPE__ Mark;    //Always be "0x1234"
			__UINT8_TYPE__ messagetype;
			__INT32_TYPE__ data_length;
		} hearder;
		union uni_payload{
			__UINT8_TYPE__ data[1];
			struct tag_CSM_BasicInfo{
				__UINT8_TYPE__ name [64];
				__UINT8_TYPE__ Address[64];
				__UINT16_TYPE__ port;
			} basicInfo;   //msgType 0x01
			struct tag_CSM_Broadcast{
				__UINT8_TYPE__ name [64];
				__UINT8_TYPE__ Address[64];
				__UINT16_TYPE__ port;
			}broadcastMsg[1];//msgType 0x02
		} payload;

	} CROSS_SVR_MSG;

#endif

#if defined(_MSC_VER)

	typedef struct tag_cross_svr_message{
		struct tag_header{
			unsigned __int16 Mark;    //Always be 0x1234
			unsigned __int8 messagetype;
			__int32 data_length;
		}  hearder;
		union uni_payload{
			unsigned __int8 data[1];
			struct tag_CSM_BasicInfo{
				unsigned __int8 name [64];
				unsigned __int8 Address[64];
				unsigned __int16 port;
			} basicInfo;
			struct tag_CSM_Broadcast{
				unsigned __int8 name [64];
				unsigned __int8 Address[64];
				unsigned __int16 port;
			} broadcastMsg[1];
		} payload;
	} CROSS_SVR_MSG;

#endif

}

#endif // CROSS_SVR_MESSAGES_H
