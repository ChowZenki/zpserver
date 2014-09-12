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



}
#pragma  pack (pop)

typedef unsigned __int32 (__stdcall * fp_st_getMACInfo)(const char * address, unsigned __int16 port,unsigned __int32 macID,ParkinglotsSvr::stMsg_GetHostDetailsRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_setHostDetails)(const char * address, unsigned __int16 port,unsigned __int32 macID, const ParkinglotsSvr::stMsg_SetHostDetailsReq * pInData,ParkinglotsSvr::stMsg_SetHostDetailsRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_removeDevice)(const char * address, unsigned __int16 port,unsigned __int32 macID, const ParkinglotsSvr::stMsg_RemoveDeviceReq * pInData,ParkinglotsSvr::stMsg_RemoveDeviceRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_getDeviceList)(const char * address, unsigned __int16 port,unsigned __int32 macID,ParkinglotsSvr::stMsg_GetDeviceListRsp ** ppOutputBuf);
typedef void (__stdcall * fp_st_freeDeviceList)(ParkinglotsSvr::stMsg_GetDeviceListRsp * pOutputBuf);



//This class help client app to get dll method easily
class pklts_ctrl{
private:
	HMODULE m_dllMod;
	fp_st_getMACInfo		m_fn_st_getMACInfo;
	fp_st_setHostDetails	m_fn_st_setHostDetails;
	fp_st_removeDevice		m_fn_st_removeDevice;
	fp_st_getDeviceList		m_fn_st_getDeviceList;
	fp_st_freeDeviceList	m_fn_st_freeDeviceList;
public:
	inline pklts_ctrl(const _TCHAR * dllFilePath)
	{
		m_dllMod = ::LoadLibrary(dllFilePath);
		if (m_dllMod !=NULL)
		{
			m_fn_st_getMACInfo = (fp_st_getMACInfo )::GetProcAddress(m_dllMod,"st_getMACInfo");
			m_fn_st_setHostDetails = (fp_st_setHostDetails )::GetProcAddress(m_dllMod,"st_setHostDetails");
			m_fn_st_removeDevice = (fp_st_removeDevice )::GetProcAddress(m_dllMod,"st_removeDevice");
			m_fn_st_getDeviceList = (fp_st_getDeviceList )::GetProcAddress(m_dllMod,"st_getDeviceList");
			m_fn_st_freeDeviceList = (fp_st_freeDeviceList )::GetProcAddress(m_dllMod,"st_freeDeviceList");
		}
		else
		{
			m_fn_st_getMACInfo = NULL;
			m_fn_st_setHostDetails = NULL;
			m_fn_st_removeDevice = NULL;
			m_fn_st_getDeviceList = NULL;
			m_fn_st_freeDeviceList = NULL;
		}
	}

	inline ~pklts_ctrl()
	{
		if (m_dllMod)
		{
			::FreeLibrary(m_dllMod);
			m_dllMod = 0;
		}
	}

	inline bool valid()
	{
		if (m_dllMod==0) return false;
		if (m_fn_st_getMACInfo == 0) return false;
		if (m_fn_st_setHostDetails == 0) return false;
		if (m_fn_st_removeDevice == 0) return false;
		if (m_fn_st_getDeviceList == 0) return false;
		if (m_fn_st_freeDeviceList == 0) return false;
		return true;
	}

public:
	inline unsigned __int32  st_getMACInfo(const char * address, unsigned __int16 port,unsigned __int32 macID, ParkinglotsSvr::stMsg_GetHostDetailsRsp * pOutputBuf)
	{
		return m_fn_st_getMACInfo(address,port,macID,pOutputBuf);
	}
	inline unsigned __int32  st_setHostDetails(const char * address, unsigned __int16 port,unsigned __int32 macID, const ParkinglotsSvr::stMsg_SetHostDetailsReq * pInData,ParkinglotsSvr::stMsg_SetHostDetailsRsp * pOutputBuf)
	{
		return m_fn_st_setHostDetails(address,port,macID,pInData,pOutputBuf);
	}
	inline unsigned __int32  st_removeDevice(const char * address, unsigned __int16 port,unsigned __int32 macID, const ParkinglotsSvr::stMsg_RemoveDeviceReq * pInData,ParkinglotsSvr::stMsg_RemoveDeviceRsp * pOutputBuf)
	{
		return m_fn_st_removeDevice(address,port,macID,pInData,pOutputBuf);
	}
	inline unsigned __int32  st_getDeviceList(const char * address, unsigned __int16 port,unsigned __int32 macID,ParkinglotsSvr::stMsg_GetDeviceListRsp ** ppOutputBuf)
	{
		return m_fn_st_getDeviceList(address,port,macID,ppOutputBuf);
	}
	inline void  st_freeDeviceList(ParkinglotsSvr::stMsg_GetDeviceListRsp * pOutputBuf)
	{
		return m_fn_st_freeDeviceList(pOutputBuf);
	}
};


#endif