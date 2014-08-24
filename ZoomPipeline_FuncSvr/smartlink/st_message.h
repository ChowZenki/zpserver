#ifndef ST_MESSAGE_H
#define ST_MESSAGE_H
#include <qglobal.h>
namespace ParkinglotsSvr{
#define MAXPARAMDATALEN 256
#define MAXPARAMNUM 8
#pragma  pack (push,1)

	//-------------------------------------------------------------------
	/**Application Layer Message Types
	  *0x100X Serial
	**/
	//stMsg_HostRegistReq , 0x1000
	struct stMsg_HostRegistReq{
		quint8 HostSerialNum[1];  /*max 64 bytes*/
	};

	//stMsg_HostRegistRsp 0x1800
	struct stMsg_HostRegistRsp{
		quint8 DoneCode;
		quint32 ID;
	};

	//User Login request
	//PKLTS_APP_LAYER::MsgType =  0x1001
	struct stMsg_HostLogonReq{
		quint32 ID;
		char HostSerialNum[1];
	};

	//User Log response
	//PKLTS_APP_LAYER::MsgType =  0x1801
	struct stMsg_HostLogonRsp{
		quint8 DoneCode;            //0- successful, 1-redirect, 3-failed.
		//quint32 UserID;
		//quint16 port_Redirect;      // and a port num.
		//quint8 Address_Redirect[64];// for server-cluster balance, may be this login should be re-direct to another address
	} ;


	//User Log response
	//SMARTLINK_MSG_APP::MsgType =  0x1002
	struct stMsg_HostTimeCorrectReq{

	} ;

	//Time Correct
	struct stMsg_HostTimeCorrectRsp{
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
	};

	//0x100B SendDeviceListReq
	struct stMsg_SendDeviceListReq{
		quint16 DeviceNums;  //device nums
		char pStrings[1];
	};

	//0x0x180B
	struct stMsg_SendDeviceListRsp{
		//Empty
	};

	//0x100C
	struct stMsg_SendMacInfoReq{
		quint16 FirmwareVersion;
		char pStart[1];
		/*
		char HostName[64];
		char HostInfo[64];
		quint8 ConnetType;	//1:GPRS,2:3G,3:WAN,4:LAN
		qint8 IEEEAdd[8];
		qint8 IEEEAdd_Flag;
		qint8 PANID[2];
		qint8 PANID_Flag;
		qint8 EPANID[8];
		qint8 EPANID_Flag;
		quint16 SensorNum;
		quint16 RelayNum;
		quint16 ANSensorNum;
		quint16 ANRelayNum;
		*/
	};
	//0x100C Internal
	struct stMsg_SendMacInfoReq_internal{
		quint16 FirmwareVersion;
		char HostName[64];
		char HostInfo[64];
		struct tag_TailData{
			quint8 ConnetType;	//1:GPRS,2:3G,3:WAN,4:LAN
			quint8 IEEEAdd[8];
			quint8 IEEEAdd_Flag;
			quint8 PANID[2];
			quint8 PANID_Flag;
			quint8 EPANID[8];
			quint8 EPANID_Flag;
			quint16 SensorNum;
			quint16 RelayNum;
			quint16 ANSensorNum;
			quint16 ANRelayNum;
		} tail_data;
	};
	//0x0x180C
	struct stMsg_SendMacInfoRsp{
		//Empty
	};


	//----------------------------------------------------------------
	//DAL is a variant-length system, so ,this sturct is only for reference.
	//struct stParam{
	//	quint8 DataType;
	//	quint8 Data[MAXPARAMDATALEN];
	//}



	//----------------------------------------------------------------
	//Event system
	//0x0000, Device Joined event
	struct stEvent_DeviceJoined{
		quint8 DeviceID[24];
	};
	//0x0001, Device Removed event
	struct stEvent_DeviceRemoved{
		quint8 DeviceID[24];
	};
	//0x0002, DAL Messages
	struct stEvent_DeviceEvent{
		quint8 DeviceID[24];
		quint8 DALEventID;
		quint8 ParamNum;
		/*stParam ParamList[MAXPARAMNUM];
		 * quint8 DataType;
		 * quint8 Data[]
		*/
	};


	struct stEvent{
		quint16 EventType;
		//Event Priority
		enum enEventPriority {
			PRI_LOW = 0,
			PRI_NORMAL =1,
			PRI_HIGH = 2
		}  Priority;
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
		quint8 DoneCode;
	} ;

	//------------------------------------------------------------------
	/**Trans Layer Structure
	**/

	struct PKLTS_Message{
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
					stMsg_SendMacInfoReq msg_stMsg_SendMacInfoReq;
					stMsg_SendMacInfoRsp msg_stMsg_SendMacInfoRsp;
					quint8 msg[1];
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
		quint16 Mark;  //Always be 0xBeBe
		quint32 source_id ; //0xffffffff means from server
		quint16 tmStamp;
	} ;

#pragma pack(pop)
	typedef PKLTS_Message::tag_trans_header PKLTS_Trans_Header;
	typedef PKLTS_Message::uni_trans_payload::tag_pklts_app_layer::tag_app_layer_header PKLTS_App_Header;
	typedef PKLTS_Message::uni_trans_payload::tag_pklts_app_layer PKLTS_App_Layer;
}
#endif // ST_MESSAGE_H
