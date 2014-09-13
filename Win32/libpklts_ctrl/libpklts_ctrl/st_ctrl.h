#ifndef ST_CTRL_H
#define ST_CTRL_H
#include <windows.h>
#include <tchar.h>
#pragma  pack (push,1)

namespace ParkinglotsSvr{
	enum ErrMessage{
		ALL_SUCCEED          = 0,
		//Net Errors, 4Bit
		ERRNET_WSAStartupFailed = 1,
		ERRNET_GetAddrInfoError = 2,
		ERRNET_SocketFailedErr  = 3,
		ERRNET_ConnectionFailed = 4,
		ERRNET_SendDataFailed   = 5,
		ERRNET_RecvDataFailed   = 6,
		ERRNET_ShutDownFailed   = 7,
		//TransLayer Errors, 4Bit
		ERRTRANS_ERROR_MARK				 =0x10,
		ERRTRANS_LESS_DATA				 =0x11,
		ERRTRANS_DST_NOT_REACHABLE		 =0x12,
		ERRTRANS_ERROR_MSG_TYPE			 =0x13,
		ERRTRANS_ERROR_DATA              =0x14
	};

	//0x2000
	struct stMsg_GetHostDetailsReq{
		
	};

	//0x2800
	struct stMsg_GetHostDetailsRsp{
		unsigned __int8 DoneCode;
		unsigned __int16 HostType;
		unsigned __int16 FirmwareVersion;
		char HostName[64];
		char HostInfo[64];
		unsigned __int8 ConnetType;
		//1£ºGPRS£¬2£º3G£¬3£ºWAN£¬4£ºLAN
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
	};
	//0x2001
	struct stMsg_SetHostDetailsReq
	{
		char HostName[64];
		char HostInfo[64];
	};
	//0x2801
	struct stMsg_SetHostDetailsRsp{
		unsigned __int8 DoneCode;
	};
	//0x200A
	struct stMsg_RemoveDeviceReq {
		unsigned __int8 DeviceID[24];
	};
	//0x280A
	struct stMsg_RemoveDeviceRsp{
		unsigned __int8 DoneCode;
	};
	//0x200B
	struct stMsg_GetDeviceListReq{

	};
	//0x280B
	struct stMsg_GetDeviceListRsp
	{
		unsigned __int8   DoneCode;
		unsigned __int16  nDevCount;
		struct   stCall_DeviceNode
		{
			char DeviceName [32];
			char No[64];
			unsigned char DeviceID[24];
		} devicetable[1];
	};
	//0x200C
	struct stMsg_GetDeviceParamReq
	{
		unsigned __int8 DeviceID[24];
		unsigned __int8 Opt_DeviceName;
		unsigned __int8 Opt_DeviceInfo;
		unsigned __int8 Opt_DALStatus;
	};
	//0x280C
	struct stMsg_GetDeviceParamRsp
	{
		unsigned __int8 DoneCode;
		unsigned char DeviceID[24];
		unsigned __int8 Opt_DeviceName;
		unsigned __int8 Opt_DeviceInfo;
		unsigned __int8 Opt_DALStatus;
		char DeviceName[32];
		char DeviceInfo[64];
		unsigned __int16 DALStatusBytesLen;
		unsigned char DALStatusBytes[1];
	};

	//0x200D
	struct stMsg_setDeviceParamReq
	{
		unsigned char DeviceID[24];
		unsigned __int8 Opt_DeviceName;
		unsigned __int8 Opt_DeviceInfo;
		char DeviceName[32];
		char DeviceInfo[64];
	};
	//0x280D
	struct stMsg_setDeviceParamRsp
	{
		unsigned __int8 DoneCode;
	};
	//0x280E
	struct stMsg_DeviceCtrlReq
	{
		unsigned char DeviceID[24];
		unsigned __int16 DALArrayLength;
	};
	//0x280E
	struct stMsg_DeviceCtrlRsp
	{
		unsigned __int8 DoneCode;
	};
}
#pragma  pack (pop)
#endif