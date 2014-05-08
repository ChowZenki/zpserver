#ifndef ST_MSG_APPLAYER_H
#define ST_MSG_APPLAYER_H

namespace ExampleServer{

#pragma  pack (push,1)

#if defined(__GNUC__)
#include <stdint.h>

	//User Login request
	//SMARTLINK_MSG_APP::MsgType =  0x0001
	typedef struct tag_stMsg_ClientLoginReq{
		__UINT32_TYPE__ user_id;
		char Passwd[1];
	}stMsg_ClientLoginReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x7FFE
	typedef struct tag_stMsg_ClientLoginRsp{
		__UINT8_TYPE__ DoneCode;
		__UINT32_TYPE__ UserID;
		__UINT8_TYPE__ Address_Redirect[64];// for server-cluster balance, may be this login should be re-direct to another address
		__UINT16_TYPE__ port_Redirect;      // and a port num.
	} stMsg_ClientLoginRsp;

	//0x1002
	typedef struct tag_stMsg_ClientLogoutReq{
		char UserName[1];//max 32
	}stMsg_ClientLogoutReq;

	//0x7FFD
	typedef struct tag_stMsg_ClientLogoutRsp{
		__UINT8_TYPE__ DoneCode;
	}stMsg_ClientLogoutRsp;


	//UploadUserListReq,0x1003
	typedef struct tag_stMsg_UploadUserListReq{
		__UINT16_TYPE__ UserNum;
		__UINT32_TYPE__ pUserIDList[1];
	}stMsg_UploadUserListReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x7FFC
	typedef struct tag_stMsg_UploadUserListRsp{
		__UINT8_TYPE__ DoneCode;
	} stMsg_UploadUserListRsp;

	//SMARTLINK_MSG_APP::MsgType =  0x1004
	typedef struct tag_stMsg_DownloadUserListReq{

	} stMsg_DownloadUserListReq;

	//SMARTLINK_MSG_APP::MsgType =  0x7FFB
	typedef struct tag_stMsg_DownloadUserListRsp{
		__UINT8_TYPE__ DoneCode;
		__UINT16_TYPE__ UserNum;
		__UINT32_TYPE__ pUserIDList[1];
	} stMsg_DownloadUserListRsp;




	typedef struct tag_example_app_layer{
		struct tag_app_layer_header{
			__UINT16_TYPE__ MsgType;
		} header;
		union  union_MsgUnion
		{
			stMsg_ClientLoginReq msg_ClientLoginReq;
			stMsg_ClientLoginRsp msg_ClientLoginRsp;
			stMsg_UploadUserListReq  msg_UploadUserListReq;
			stMsg_UploadUserListRsp  msg_UploadUserListRsp;
			stMsg_DownloadUserListReq msg_DownloadUserListReq;
			stMsg_DownloadUserListRsp msg_DownloadUserListRsp;
			stMsg_ClientLogoutReq msg_ClientLogoutReq;
			stMsg_ClientLogoutRsp msg_ClientLogoutRsp;
		}MsgUnion;
	} EXAMPLE_MSG_APP;

#endif

#if defined(_MSC_VER)

	//User Login request
	//SMARTLINK_MSG_APP::MsgType =  0x0001
	typedef struct tag_stMsg_ClientLoginReq{
		unsigned __int32 user_id;
		char Passwd[1];
	}stMsg_ClientLoginReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x7FFE
	typedef struct tag_stMsg_ClientLoginRsp{
		unsigned __int8 DoneCode;            //0- successful, 1-redirect, 3-failed.
		unsigned __int32 UserID;
		unsigned __int8 Address_Redirect[64];// for server-cluster balance, may be this login should be re-direct to another address
		unsigned __int16 port_Redirect;      // and a port num.
	} stMsg_ClientLoginRsp;

	//0x1002
	typedef struct tag_stMsg_ClientLogoutReq{
		char UserName[1];//max 32
	}stMsg_ClientLogoutReq;

	//0x7FFD
	typedef struct tag_stMsg_ClientLogoutRsp{
		unsigned __int8 DoneCode;
	}stMsg_ClientLogoutRsp;

	//UploadUserListReq,0x1003
	typedef struct tag_stMsg_UploadUserListReq{
		unsigned __int16 UserNum;
		unsigned __int32 pUserIDList[1];
	}stMsg_UploadUserListReq;

	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x7FFC
	typedef struct tag_stMsg_UploadUserListRsp{
		unsigned __int8 DoneCode;
		//char TextInfo[64];
	} stMsg_UploadUserListRsp;


	//SMARTLINK_MSG_APP::MsgType =  0x1004
	typedef struct tag_stMsg_DownloadUserListReq{

	} stMsg_DownloadUserListReq;

	//SMARTLINK_MSG_APP::MsgType =  0x7FFB
	typedef struct tag_stMsg_DownloadUserListRsp{
		unsigned __int8 DoneCode;
		unsigned __int16 UserNum;
		unsigned __int32 pUserIDList[1];
	} stMsg_DownloadUserListRsp;



	typedef struct tag_example_app_layer{
		struct tag_app_layer_header{
			unsigned __int16 MsgFmtVersion;
			unsigned __int8 AskID;
			unsigned __int16 MsgType;
		} header;
		union  union_MsgUnion
		{
			stMsg_ClientLoginReq msg_ClientLoginReq;
			stMsg_ClientLoginRsp msg_ClientLoginRsp;
			stMsg_UploadUserListReq  msg_UploadUserListReq;
			stMsg_UploadUserListRsp  msg_UploadUserListRsp;
			stMsg_DownloadUserListReq msg_DownloadUserListReq;
			stMsg_DownloadUserListRsp msg_DownloadUserListRsp;
			stMsg_ClientLogoutReq msg_ClientLogoutReq;
			stMsg_ClientLogoutRsp msg_ClientLogoutRsp;

		}MsgUnion;

	} EXAMPLE_MSG_APP;

#endif



#pragma pack(pop)

}
#endif // ST_MSG_APPLAYER_H
