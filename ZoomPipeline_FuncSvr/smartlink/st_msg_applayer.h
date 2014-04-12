#ifndef ST_MSG_APPLAYER_H
#define ST_MSG_APPLAYER_H

namespace SmartLink{

#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>

	//Host Reg request, send 64 byte Serial Num to Server
	//SMARTLINK_MSG_APP::MsgType =  0x1000
	typedef struct tag_stMsg_HostRegistReq{
		char HostSerialNum[1];//max 64
	}stMsg_HostRegistReq;

	//Host Reg replay, send back 4 byte ID
	//SMARTLINK_MSG_APP::MsgType =  0x1800
	typedef struct tag_stMsg_HostRegistRsp{
		__UINT8_TYPE__ DoneCode;//0 first ok, 1 ok 2 failed
		//char TextInfo[64];
		__UINT32_TYPE__ ID;
	}stMsg_HostRegistRsp;

	//Host Log request
	//SMARTLINK_MSG_APP::MsgType =  0x1001
	typedef struct tag_stMsg_HostLogonReq{
		__UINT32_TYPE__ ID;
		char HostSerialNum[1];//max 64
	} stMsg_HostLogonReq;

	//Host Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1801
	typedef struct tag_stMsg_HostLogonRsp{
		__UINT8_TYPE__ DoneCode;
		//char TextInfo[64];
	} stMsg_HostLogonRsp;

	//User Login request
	//SMARTLINK_MSG_APP::MsgType =  0x3000
	typedef struct tag_stMsg_ClientLoginReq{
		__UINT16_TYPE__ ClientVersion;
		char UserNameAndPasswd[1];
		//char Password[32];
	}stMsg_ClientLoginReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x3800
	typedef struct tag_stMsg_ClientLoginRsp{
		__UINT8_TYPE__ DoneCode;
		__UINT32_TYPE__ UserID;
		//char TextInfo[64];
	} stMsg_ClientLoginRsp;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1002
	typedef struct tag_stMsg_HostTimeCorrectReq{

	} stMsg_HostTimeCorrectReq;

	//Time Correct
	typedef struct tag_stMsg_HostTimeCorrectRsp{
		__UINT8_TYPE__ DoneCode;
		//char TextInfo[64];
		struct tag_stDateTime{
			__UINT16_TYPE__ Year;
			__UINT8_TYPE__ Month;
			__UINT8_TYPE__ Day;
			__UINT8_TYPE__ Hour;
			__UINT8_TYPE__ Minute;
			__UINT8_TYPE__ Second;
		} DateTime;
	}stMsg_HostTimeCorrectRsp;

	//UploadUserListReq,0x1003
	typedef struct tag_stMsg_UploadUserListReq{
		__UINT16_TYPE__ UserNum;
		__UINT32_TYPE__ pUserIDList[1];
	}stMsg_UploadUserListReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1803
	typedef struct tag_stMsg_UploadUserListRsp{
		__UINT8_TYPE__ DoneCode;
		//char TextInfo[64];
	} stMsg_UploadUserListRsp;

	//SMARTLINK_MSG_APP::MsgType =  0x1004
	typedef struct tag_stMsg_DownloadUserListReq{

	} stMsg_DownloadUserListReq;

	//SMARTLINK_MSG_APP::MsgType =  0x1804
	typedef struct tag_stMsg_DownloadUserListRsp{
		__UINT8_TYPE__ DoneCode;
		//char TextInfo[64];
		__UINT16_TYPE__ UserNum;
		__UINT32_TYPE__ pUserIDList[1];
	} stMsg_DownloadUserListRsp;


	//0x3001
	typedef struct tag_stMsg_ClientLogoutReq{
		char UserName[1];//max 32
	}stMsg_ClientLogoutReq;

	//0x3801
	typedef struct tag_stMsg_ClientLogoutRsp{
		__UINT8_TYPE__ DoneCode;
		//char TextInfo[64];
	}stMsg_ClientLogoutRsp;

	//SMARTLINK_MSG_APP::MsgType =  0x3002
	typedef struct tag_stMsg_GetHostListReq{

	} stMsg_GetHostListReq;
	//SMARTLINK_MSG_APP::MsgType =  0x3802
	typedef struct  tag_stMsg_GetHostListRsp{
		__UINT32_TYPE__ HostNum;
		__UINT32_TYPE__ pHostIDList[1];
	}stMsg_GetHostListRsp;

	typedef struct tag_smartlink_app_layer{
		struct tag_app_layer_header{
			__UINT16_TYPE__ MsgFmtVersion;
			__UINT8_TYPE__ AskID;
			__UINT16_TYPE__ MsgType;
		} header;
		union  union_MsgUnion
		{
			stMsg_HostRegistReq msg_HostRegistReq;
			stMsg_HostRegistRsp msg_HostRegistRsp;
			stMsg_HostLogonReq msg_HostLogonReq;
			stMsg_HostLogonRsp msg_HostLogonRsp;
			stMsg_ClientLoginReq msg_ClientLoginReq;
			stMsg_ClientLoginRsp msg_ClientLoginRsp;
			stMsg_HostTimeCorrectReq msg_HostTimeCorrectReq;
			stMsg_HostTimeCorrectRsp msg_HostTimeCorrectRsp;
			stMsg_UploadUserListReq  msg_UploadUserListReq;
			stMsg_UploadUserListRsp  msg_UploadUserListRsp;
			stMsg_DownloadUserListReq msg_DownloadUserListReq;
			stMsg_DownloadUserListRsp msg_DownloadUserListRsp;
			stMsg_ClientLogoutReq msg_ClientLogoutReq;
			stMsg_ClientLogoutRsp msg_ClientLogoutRsp;
			stMsg_GetHostListReq msg_GetHostListReq;
			stMsg_GetHostListRsp msg_GetHostListRsp;
		}MsgUnion;
	} SMARTLINK_MSG_APP;

#endif

#if defined(_MSC_VER)

	//Host Reg request, send 64 byte Serial Num to Server
	//SMARTLINK_MSG_APP::MsgType =  0x1000
	typedef struct tag_stMsg_HostRegistReq{
		char HostSerialNum[1];//max 64
	}stMsg_HostRegistReq;


	//Host Reg replay, send back 4 byte ID
	//SMARTLINK_MSG_APP::MsgType =  0x1800
	typedef struct tag_stMsg_HostRegistRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
		unsigned __int32 ID;
	}stMsg_HostRegistRsp;

	//Host Log request
	//SMARTLINK_MSG_APP::MsgType =  0x1001
	typedef struct tag_stMsg_HostLogonReq{
		unsigned __int32 ID;
		char HostSerialNum[1];//max 64
	} stMsg_HostLogonReq;

	//Host Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1801
	typedef struct tag_stMsg_HostLogonRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
	} stMsg_HostLogonRsp;

	//User Login request
	//SMARTLINK_MSG_APP::MsgType =  0x3000
	typedef struct tag_stMsg_ClientLoginReq{
		unsigned __int16 ClientVersion;
		char UserNameAndPasswd[1];//max 65
		//char Password[1];//max 32
	}stMsg_ClientLoginReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x3800
	typedef struct tag_stMsg_ClientLoginRsp{
		unsigned __int8 DoneCode;
		unsigned __int32 UserID;
		//char TextInfo[64];
	} stMsg_ClientLoginRsp;


	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1002
	typedef struct tag_stMsg_HostTimeCorrectReq{

	} stMsg_HostTimeCorrectReq;

	//Time Correct
	typedef struct tag_stMsg_HostTimeCorrectRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
		struct tag_stDateTime{
			unsigned __int16 Year;
			unsigned __int8 Month;
			unsigned __int8 Day;
			unsigned __int8 Hour;
			unsigned __int8 Minute;
			unsigned __int8 Second;
		} DateTime;
	}stMsg_HostTimeCorrectRsp;

	//UploadUserListReq,0x1003
	typedef struct tag_stMsg_UploadUserListReq{
		unsigned __int16 UserNum;
		unsigned __int32 pUserIDList[1];
	}stMsg_UploadUserListReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1803
	typedef struct tag_stMsg_UploadUserListRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
	} stMsg_UploadUserListRsp;


	//SMARTLINK_MSG_APP::MsgType =  0x1004
	typedef struct tag_stMsg_DownloadUserListReq{

	} stMsg_DownloadUserListReq;

	//SMARTLINK_MSG_APP::MsgType =  0x1804
	typedef struct tag_stMsg_DownloadUserListRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
		unsigned __int16 UserNum;
		unsigned __int32 pUserIDList[1];
	} stMsg_DownloadUserListRsp;

	//0x3001
	typedef struct tag_stMsg_ClientLogoutReq{
		char UserName[1];//max 32
	}stMsg_ClientLogoutReq;

	//0x3801
	typedef struct tag_stMsg_ClientLogoutRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
	}stMsg_ClientLogoutRsp;

	//SMARTLINK_MSG_APP::MsgType =  0x3002
	typedef struct tag_stMsg_GetHostListReq{

	} stMsg_GetHostListReq;
	//SMARTLINK_MSG_APP::MsgType =  0x3802
	typedef struct  tag_stMsg_GetHostListRsp{
		quint32 HostNum;
		quint32 pHostIDList[1];
	}stMsg_GetHostListRsp;

	typedef struct tag_smartlink_app_layer{
		struct tag_app_layer_header{
			unsigned __int16 MsgFmtVersion;
			unsigned __int8 AskID;
			unsigned __int16 MsgType;
		} header;
		union  union_MsgUnion
		{
			stMsg_HostRegistReq msg_HostRegistReq;
			stMsg_HostRegistRsp msg_HostRegistRsp;
			stMsg_HostLogonReq msg_HostLogonReq;
			stMsg_HostLogonRsp msg_HostLogonRsp;
			stMsg_ClientLoginReq msg_ClientLoginReq;
			stMsg_ClientLoginRsp msg_ClientLoginRsp;
			stMsg_HostTimeCorrectReq msg_HostTimeCorrectReq;
			stMsg_HostTimeCorrectRsp msg_HostTimeCorrectRsp;
			stMsg_UploadUserListReq  msg_UploadUserListReq;
			stMsg_UploadUserListRsp  msg_UploadUserListRsp;
			stMsg_DownloadUserListReq msg_DownloadUserListReq;
			stMsg_DownloadUserListRsp msg_DownloadUserListRsp;
			stMsg_ClientLogoutReq msg_ClientLogoutReq;
			stMsg_ClientLogoutRsp msg_ClientLogoutRsp;
			stMsg_GetHostListReq msg_GetHostListReq;
			stMsg_GetHostListRsp msg_GetHostListRsp;

		}MsgUnion;

	} SMARTLINK_MSG_APP;

#endif



#pragma pack(pop)

}
#endif // ST_MSG_APPLAYER_H
