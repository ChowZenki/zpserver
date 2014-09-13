#ifndef ST_CTRLMESSAGE_H
#define ST_CTRLMESSAGE_H
#include "st_ctrl.h"
namespace ParkinglotsSvr{
#define MAXPARAMDATALEN 256
#define MAXPARAMNUM 8

#pragma  pack (push,1)

	enum dal_datatype{
		DAL_TYPE_NODATA	=0x00,
		DAL_TYPE_BOOL	=0x10,
		DAL_TYPE_UINT8	=0x20,
		DAL_TYPE_UINT16	=0x21,
		DAL_TYPE_UINT24	=0x22,
		DAL_TYPE_UINT32	=0x23,
		DAL_TYPE_UINT40	=0x24,
		DAL_TYPE_UINT48	=0x25,
		DAL_TYPE_UINT56	=0x26,
		DAL_TYPE_UINT64	=0x27,
		DAL_TYPE_INT8	=0x28,
		DAL_TYPE_INT16	=0x29,
		DAL_TYPE_INT24	=0x2A,
		DAL_TYPE_INT32	=0x2B,
		DAL_TYPE_INT40	=0x2C,
		DAL_TYPE_INT48	=0x2D,
		DAL_TYPE_INT56	=0x2E,
		DAL_TYPE_INT64	=0x2F,
		DAL_TYPE_ENUM8	=0x30,
		DAL_TYPE_SINGLEFLOAT	=0x39,
		DAL_TYPE_DOUBLEFLOAT	=0x3A,
		DAL_TYPE_STRING	=0x42
	};

	//-------------------------------------------------------------------
	/**Application Layer Message Types
	  *0x100X Serial
	**/
	//stMsg_HostRegistReq , 0x1000
	struct stMsg_HostRegistReq{
		unsigned __int8 HostSerialNum[1];  /*max 64 bytes*/
	};

	//stMsg_HostRegistRsp 0x1800
	struct stMsg_HostRegistRsp{
		unsigned __int8 DoneCode;
		unsigned __int32 ID;
	};

	//User Login request
	//PKLTS_APP_LAYER::MsgType =  0x1001
	struct stMsg_HostLogonReq{
		unsigned __int32 ID;
		char HostSerialNum[1];
	};

	//User Log response
	//PKLTS_APP_LAYER::MsgType =  0x1801
	struct stMsg_HostLogonRsp{
		unsigned __int8 DoneCode;            //0- successful, 1-redirect, 3-failed.
		//unsigned __int32 UserID;
		//unsigned __int16 port_Redirect;      // and a port num.
		//unsigned __int8 Address_Redirect[64];// for server-cluster balance, may be this login should be re-direct to another address
	} ;


	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1002
	struct stMsg_HostTimeCorrectReq{

	} ;

	//Time Correct
	struct stMsg_HostTimeCorrectRsp{
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
	};

	//0x100B SendDeviceListReq
	struct stMsg_SendDeviceListReq{
		unsigned __int16 DeviceNums;  //device nums
		char pStrings[1];
	};

	//0x0x180B
	struct stMsg_SendDeviceListRsp{
		//Empty
	};

	//0x100C
	struct stMsg_SendMacInfoReq{
		unsigned __int16 FirmwareVersion;
		char pStart[1];
		/*
		char HostName[64];
		char HostInfo[64];
		unsigned __int8 ConnetType;	//1:GPRS,2:3G,3:WAN,4:LAN
		__int8 IEEEAdd[8];
		__int8 IEEEAdd_Flag;
		__int8 PANID[2];
		__int8 PANID_Flag;
		__int8 EPANID[8];
		__int8 EPANID_Flag;
		unsigned __int16 SensorNum;
		unsigned __int16 RelayNum;
		unsigned __int16 ANSensorNum;
		unsigned __int16 ANRelayNum;
		*/
	};
	//0x100C Internal
	struct stMsg_SendMacInfoReq_internal{
		unsigned __int16 FirmwareVersion;
		char HostName[64];
		char HostInfo[64];
		struct tag_TailData{
			unsigned __int8 ConnetType;	//1:GPRS,2:3G,3:WAN,4:LAN
			unsigned __int8 IEEEAdd[8];
			unsigned __int8 IEEEAdd_Flag;
			unsigned __int8 PANID[2];
			unsigned __int8 PANID_Flag;
			unsigned __int8 EPANID[8];
			unsigned __int8 EPANID_Flag;
			unsigned __int16 SensorNum;
			unsigned __int16 RelayNum;
			unsigned __int16 ANSensorNum;
			unsigned __int16 ANRelayNum;
		} tail_data;
	};
	//0x0x180C
	struct stMsg_SendMacInfoRsp{
		//Empty
	};




	//----------------------------------------------------------------
	//DAL is a variant-length system, so ,this sturct is only for reference.
	//struct stParam{
	//	unsigned __int8 DataType;
	//	unsigned __int8 Data[MAXPARAMDATALEN];
	//}



	//----------------------------------------------------------------
	//Event system
	//0x0000, Device Joined event
	struct stEvent_DeviceJoined{
		unsigned __int8 DeviceID[24];
	};
	//0x0001, Device Removed event
	struct stEvent_DeviceRemoved{
		unsigned __int8 DeviceID[24];
	};
	//0x0002, DAL Messages
	struct stEvent_DeviceEvent{
		unsigned __int8 DeviceID[24];
		unsigned __int8 DALEventID;
		unsigned __int8 ParamNum;
		/*stParam ParamList[MAXPARAMNUM];
		 * unsigned __int8 DataType;
		 * unsigned __int8 Data[]
		*/
	};
	//0x0003, Device Exception Report
	struct stEvent_DeviceException{
		unsigned __int8 DeviceID[24];
		unsigned __int8 ExceptionID;
		unsigned __int8 ParamNum;
		/*stParam ParamList[MAXPARAMNUM];
		 * unsigned __int8 DataType;
		 * unsigned __int8 Data[]
		*/
	};

	struct stEvent{
		unsigned __int16 EventType;
		//Event Priority
		unsigned __int8  Priority;
		union union_Event{
			stEvent_DeviceJoined evt_DeviceJoined;
			stEvent_DeviceRemoved evt_DeviceRemoved;
			stEvent_DeviceEvent evt_DeviceEvent;
		} unEvent;
	};



	//------------------------------------------------------------------
	/**Application Layer Message Types
	  *0x400X Serial
	**/	//0x4000
	struct stMsg_EventPushReq{
		stEvent event;
	} ;

	struct stMsg_EventPushRsp{
		unsigned __int8 DoneCode;
	} ;

	//------------------------------------------------------------------
	/**Trans Layer Structure
	**/

	struct PKLTS_Message{
		struct tag_trans_header{
			unsigned __int16 Mark;    //Always be 0xAA55
			unsigned __int16 ProtoVersion;
			unsigned __int8 Priority;
			unsigned __int8 Reserved1;
			unsigned __int16 SerialNum;
			unsigned __int32 SrcID ; //0xffffffff means from server
			unsigned __int32 DstID ; //0xffffffff means to server
			unsigned __int16 DataLen; //Data at least have 2 byte, for heart-beating counter.
			unsigned __int16 Reserved2;
		} trans_header;
		union uni_trans_payload{
			unsigned __int8 data[1];
			struct tag_pklts_app_layer{
				struct tag_app_layer_header{
					unsigned __int16 MsgFmtVersion;
					unsigned __int8 AskID;
					unsigned __int16 MsgType;
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
					stMsg_SendMacInfoReq msg_stMsg_SendMacInfoReq;
					stMsg_SendMacInfoRsp msg_stMsg_SendMacInfoRsp;
					stMsg_EventPushReq msg_stMsg_EventPushReq;
					stMsg_EventPushRsp msg_stMsg_EventPushRsp;

					stMsg_GetHostDetailsReq msg_GetHostDetailsReq;
					stMsg_GetHostDetailsRsp msg_GetHostDetailsRsp;
					stMsg_SetHostDetailsReq msg_SetHostDetailsReq;
					stMsg_SetHostDetailsRsp msg_SetHostDetailsRsp;
					stMsg_RemoveDeviceReq   msg_RemoveDeviceReq;
					stMsg_RemoveDeviceRsp   msg_RemoveDeviceRsp;
					stMsg_GetDeviceParamReq	msg_GetDeviceParamReq;
					stMsg_GetDeviceParamRsp	msg_GetDeviceParamRsp;
					stMsg_setDeviceParamReq msg_setDeviceParamReq;
					stMsg_setDeviceParamRsp msg_setDeviceParamRsp;
					stMsg_DeviceCtrlReq msg_DeviceCtrlReq;
					stMsg_DeviceCtrlRsp msg_DeviceCtrlRsp;
					unsigned __int8 msg[1];
				} app_data;
			} app_layer;
		} trans_payload;
	} ;
	//------------------------------------------------------------------
	/**Trans Layer Message Type
	 * Heartbeating message
	**/
	struct PKLTS_Heartbeating
	{
		unsigned __int16 Mark;  //Always be 0xBeBe
		//unsigned __int32 source_id ; //0xffffffff means from server
		unsigned __int16 tmStamp;
	} ;

#pragma pack(pop)
	typedef PKLTS_Message::tag_trans_header PKLTS_Trans_Header;
	typedef PKLTS_Message::uni_trans_payload::tag_pklts_app_layer::tag_app_layer_header PKLTS_App_Header;
	typedef PKLTS_Message::uni_trans_payload::tag_pklts_app_layer PKLTS_App_Layer;
}

#endif // ST_MESSAGE_H


