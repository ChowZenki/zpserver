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
		} hearder;
		union uni_payload{
			struct tag_plainData{
				__UINT16_TYPE__ data_length;
				__UINT8_TYPE__ data[1];
			} plainData;
		} payload;

	} CROSS_SVR_MSG;

#endif

#if defined(_MSC_VER)

	typedef struct tag_cross_svr_message{
		struct tag_header{
			unsigned __int16 Mark;    //Always be 0x1234
			unsigned __int8 messagetype;

		}  hearder;
		union uni_payload{
			struct tag_plainData{
				unsigned __int16 data_length;
				unsigned __int8 data[1];
			} plainData;

		} payload;
	} CROSS_SVR_MSG;

#endif

}

#endif // CROSS_SVR_MESSAGES_H
