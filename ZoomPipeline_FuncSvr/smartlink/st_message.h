#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H
#include <qglobal.h>
namespace ParkinglotsSvr{

#pragma  pack (push,1)

	//stMsg_HostRegistReq , 0x1000
	typedef struct tag_stMsg_HostRegistReq{
		quint8 HostSerialNum[1];  /*max 64 bytes*/
	}stMsg_HostRegistReq;

	//stMsg_HostRegistRsp 0x1800
	typedef struct tag_stMsg_HostRegistRsp{
		quint8 DoneCode;
		quint32 ID;
	}stMsg_HostRegistRsp;

	//User Login request
	//PKLTS_APP_LAYER::MsgType =  0x1001
	typedef struct tag_stMsg_HostLogonReq{
		quint32 ID;
		char HostSerialNum[1];
	}stMsg_HostLogonReq;

	//User Log response
	//PKLTS_APP_LAYER::MsgType =  0x1801
	typedef struct tag_stMsg_HostLogonRsp{
		quint8 DoneCode;            //0- successful, 1-redirect, 3-failed.
		//quint32 UserID;
		//quint16 port_Redirect;      // and a port num.
		//quint8 Address_Redirect[64];// for server-cluster balance, may be this login should be re-direct to another address
	} stMsg_HostLogonRsp;


	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1002
	typedef struct tag_stMsg_HostTimeCorrectReq{

	} stMsg_HostTimeCorrectReq;

	//Time Correct
	typedef struct tag_stMsg_HostTimeCorrectRsp{
		quint8 DoneCode;
		//char TextInfo[64];
		struct tag_stDateTime{
			quint16 Year;
			quint8 Month;
			quint8 Day;
			quint8 Hour;
			quint8 Minute;
			quint8 Second;
		} DateTime;
	}stMsg_HostTimeCorrectRsp;

	//0x100B SendDeviceListReq
	typedef struct tag_stMsg_SendDeviceListReq{
		quint16 DeviceNums;  //device nums
		char pStrings[1];
	}stMsg_SendDeviceListReq;

	//0x0x180B
	typedef struct tag_stMsg_SendDeviceListRsp{
		//空
	}stMsg_SendDeviceListRsp;



	typedef struct tag_pklts_msg{
		struct tag_trans_header{
			quint16 Mark;    //Always be 0xAA55
			quint16 ProtoVersion;
			quint8 Priority;
			quint8 Reserved1;
			quint16 SerialNum;
			quint32 SrcID ; //0xffffffff means from server
			quint32 DstID ; //0xffffffff means to server
			quint16 DataLen; //Data at least have 2 byte, for heart-beating counter.
			quint16 Reserved2;
		} trans_header;
		union uni_trans_payload{
			quint8 data[1];
			struct tag_pklts_app_layer{
				struct tag_app_layer_header{
					quint16 MsgFmtVersion;
					quint8 AskID;
					quint16 MsgType;
				} app_header;
				union  union_MsgUnion
				{
					stMsg_HostRegistReq msg_HostRegistReq;
					stMsg_HostRegistRsp msg_HostRegistRsp;
					stMsg_HostLogonReq msg_HostLogonReq;
					stMsg_HostLogonRsp msg_HostLogonRsp;
					stMsg_HostTimeCorrectReq msg_HostTimeCorrectReq;
					stMsg_HostTimeCorrectRsp msg_HostTimeCorrectRsp;
					stMsg_SendDeviceListReq msg_SendDeviceListReq;
					stMsg_SendDeviceListRsp msg_SendDeviceListRsp;
					quint8 msg[1];
				} app_data;
			} app_layer;
		} trans_payload;
	} PKLTS_MSG;

	typedef struct tag_pklts_heartBeating
	{
		quint16 Mark;  //Always be 0xBeBe
		quint32 source_id ; //0xffffffff means from server
		quint16 tmStamp;
	} PKLTS_HEARTBEATING;

#pragma pack(pop)
	typedef PKLTS_MSG::tag_trans_header PKLTS_TRANS_HEADER;
	typedef PKLTS_MSG::uni_trans_payload::tag_pklts_app_layer::tag_app_layer_header PKLTS_APP_HEADER;
	typedef PKLTS_MSG::uni_trans_payload::tag_pklts_app_layer PKLTS_APP_LAYER;
}
#endif // ST_MESSAGE_H
