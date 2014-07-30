#ifndef ST_MSG_APPLAYER_H
#define ST_MSG_APPLAYER_H
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

	//0x1002
	typedef struct tag_stMsg_ClientLogoutReq{
		char UserName[1];//max 32
	}stMsg_ClientLogoutReq;

	//0x7FFD
	typedef struct tag_stMsg_ClientLogoutRsp{
		quint8 DoneCode;
	}stMsg_ClientLogoutRsp;

	//UploadUserListReq,0x1003
	typedef struct tag_stMsg_UploadUserListReq{
		quint16 UserNum;
		quint32 pUserIDList[1];
	}stMsg_UploadUserListReq;

	//User Log response
	//PKLTS_APP_LAYER::MsgType =  0x7FFC
	typedef struct tag_stMsg_UploadUserListRsp{
		quint8 DoneCode;
		//char TextInfo[64];
	} stMsg_UploadUserListRsp;

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
	//PKLTS_APP_LAYER::MsgType =  0x1004
	typedef struct tag_stMsg_DownloadUserListReq{

	} stMsg_DownloadUserListReq;

	//PKLTS_APP_LAYER::MsgType =  0x7FFB
	typedef struct tag_stMsg_DownloadUserListRsp{
		quint8 DoneCode;
		quint16 UserNum;
		quint32 pUserIDList[1];
	} stMsg_DownloadUserListRsp;



	typedef struct tag_pklts_app_layer{
		struct tag_app_layer_header{
			quint16 MsgFmtVersion;
			quint8 AskID;
			quint16 MsgType;
		} header;
		union  union_MsgUnion
		{
			stMsg_HostRegistReq msg_HostRegistReq;
			stMsg_HostRegistRsp msg_HostRegistRsp;
			stMsg_HostLogonReq msg_HostLogonReq;
			stMsg_HostLogonRsp msg_HostLogonRsp;
			stMsg_UploadUserListReq  msg_UploadUserListReq;
			stMsg_UploadUserListRsp  msg_UploadUserListRsp;
			stMsg_DownloadUserListReq msg_DownloadUserListReq;
			stMsg_DownloadUserListRsp msg_DownloadUserListRsp;
			stMsg_ClientLogoutReq msg_ClientLogoutReq;
			stMsg_ClientLogoutRsp msg_ClientLogoutRsp;
			stMsg_HostTimeCorrectReq msg_HostTimeCorrectReq;
			stMsg_HostTimeCorrectRsp msg_HostTimeCorrectRsp;

		}MsgUnion;

	} PKLTS_APP_LAYER;



#pragma pack(pop)

}
#endif // ST_MSG_APPLAYER_H
