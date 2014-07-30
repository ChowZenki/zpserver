#ifndef ST_CROSS_SVR_MSG_H
#define ST_CROSS_SVR_MSG_H
#include <qglobal.h>
namespace ParkinglotsSvr{

#pragma  pack (push,1)
	typedef struct tag_example_crosssvr_msg{
		struct tag_msgHearder{
			quint16  Mark;    //Always be 0x4567
			quint16  version; //Structure Version
			quint8  mesageType;
			quint32  messageLen;
		} header;
		union union_payload{
			quint8  data[1];
			quint32  uuids[1];
		} payload;
	} PKLTS_CROSSSVR_MSG;

#pragma pack(pop)
}
#endif
