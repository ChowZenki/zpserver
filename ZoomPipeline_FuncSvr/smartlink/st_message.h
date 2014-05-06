#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H

namespace ExampleServer{

#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>
	/**
	 * @brief this structure provides a basic trans blocks.
	 *
	 */
	typedef struct tag_example_message{
		__UINT16_TYPE__ Mark;    //Always be "0xAA55"
		__UINT32_TYPE__ source_id ; //0xffffffff means from server
		__UINT32_TYPE__ destin_id ; //0xffffffff means to server
		__UINT16_TYPE__ data_length; //Data at least have 2 byte, for heart-beating counter.
		__UINT8_TYPE__ data[1];
	} EXAMPLE_TRANS_MSG;
	typedef struct tag_example_heartBeating
	{
		__UINT16_TYPE__ Mark;  //Always be 0xBeBe
		__UINT16_TYPE__ tmStamp;
	} EXAMPLE_HEARTBEATING;
#endif

#if defined(_MSC_VER)
	typedef struct tag_example_message{
		unsigned __int16 Mark;    //Always be 0xAA55
		unsigned __int32 source_id ; //0xffffffff means from server
		unsigned __int32 destin_id ; //0xffffffff means to server
		unsigned __int16 data_length; //Data at least have 2 byte, for heart-beating counter.
		unsigned __int8 data[1];
	} EXAMPLE_TRANS_MSG;
	typedef struct tag_example_heartBeating
	{
		unsigned __int16 Mark;  //Always be 0xBeBe
		unsigned __int16 tmStamp;
	} EXAMPLE_HEARTBEATING;
#endif



#pragma pack(pop)
}
#endif // ST_MESSAGE_H
