#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H
#include <qglobal.h>
namespace ParkinglotsSvr{

#pragma  pack (push,1)

	typedef struct tag_pklts_message{
		quint16 Mark;    //Always be 0xAA55
		quint16 ProtoVersion;
		quint8 Priority;
		quint8 Reserved1;
		quint16 SerialNum;
		quint32 SrcID ; //0xffffffff means from server
		quint32 DstID ; //0xffffffff means to server
		quint16 DataLen; //Data at least have 2 byte, for heart-beating counter.
		quint16 Reserved2;
		quint8 data[1];
	} PKLTS_TRANS_MSG;
	typedef struct tag_pklts_heartBeating
	{
		quint16 Mark;  //Always be 0xBeBe
		quint32 source_id ; //0xffffffff means from server
		quint16 tmStamp;
	} PKLTS_HEARTBEATING;

#pragma pack(pop)
}
#endif // ST_MESSAGE_H
