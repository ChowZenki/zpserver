#include "../libpklts_ctrl/st_ctrl.h"
using namespace ParkinglotsSvr;
typedef unsigned __int32 (__stdcall * fp_st_getMACInfo)(const char * address, unsigned __int16 port,unsigned __int32 macID,stMsg_GetHostDetailsRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_setHostDetails)(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_SetHostDetailsReq * pInData,stMsg_SetHostDetailsRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_removeDevice)(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_RemoveDeviceReq * pInData,stMsg_RemoveDeviceRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_getDeviceList)(const char * address, unsigned __int16 port,unsigned __int32 macID,stMsg_GetDeviceListRsp ** ppOutputBuf);
typedef void (__stdcall * fp_st_freeDeviceList)(stMsg_GetDeviceListRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_getDeviceParam)(const char * address,unsigned __int16 port,unsigned __int32 macID,const stMsg_GetDeviceParamReq * pInBuf,stMsg_GetDeviceParamRsp ** ppOutputBuf);
typedef void (__stdcall * fp_st_freeDeviceParam)(stMsg_GetDeviceParamRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_setDeviceParam)(const char * address,unsigned __int16 port,unsigned __int32 macID,const stMsg_setDeviceParamReq * pInData,stMsg_setDeviceParamRsp * pOutputBuf);
typedef unsigned __int32 (__stdcall * fp_st_deviceCtrl)(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_DeviceCtrlReq * pInData,const unsigned __int8 * pDAL,stMsg_DeviceCtrlRsp * pOutputBuf);

//This class help client app to get dll method easily
class pklts_ctrl{
private:
	HMODULE m_dllMod;
	fp_st_getMACInfo		m_fn_st_getMACInfo;
	fp_st_setHostDetails	m_fn_st_setHostDetails;
	fp_st_removeDevice		m_fn_st_removeDevice;
	fp_st_getDeviceList		m_fn_st_getDeviceList;
	fp_st_freeDeviceList	m_fn_st_freeDeviceList;
	fp_st_getDeviceParam	m_fn_st_getDeviceParam;
	fp_st_freeDeviceParam	m_fn_st_freeDeviceParam;
	fp_st_setDeviceParam	m_fn_st_setDeviceParam;
	fp_st_deviceCtrl		m_fn_st_deviceCtrl;
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
			m_fn_st_getDeviceParam = (fp_st_getDeviceParam )::GetProcAddress(m_dllMod,"st_getDeviceParam");
			m_fn_st_freeDeviceParam = (fp_st_freeDeviceParam )::GetProcAddress(m_dllMod,"st_freeDeviceParam");
			m_fn_st_setDeviceParam = (fp_st_setDeviceParam )::GetProcAddress(m_dllMod,"st_setDeviceParam");
			m_fn_st_deviceCtrl = (fp_st_deviceCtrl )::GetProcAddress(m_dllMod,"st_deviceCtrl");
		}
		else
		{
			m_fn_st_getMACInfo = NULL;
			m_fn_st_setHostDetails = NULL;
			m_fn_st_removeDevice = NULL;
			m_fn_st_getDeviceList = NULL;
			m_fn_st_freeDeviceList = NULL;
			m_fn_st_getDeviceParam = NULL;
			m_fn_st_freeDeviceParam = NULL;
			m_fn_st_setDeviceParam = NULL;
			m_fn_st_deviceCtrl = NULL;
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
		if (m_fn_st_getDeviceParam == 0) return false;
		if (m_fn_st_freeDeviceParam == 0) return false;
		if (m_fn_st_setDeviceParam == 0) return false;
		if (m_fn_st_deviceCtrl == 0) return false;
		return true;
	}

public:
	inline unsigned __int32  st_getMACInfo(const char * address, unsigned __int16 port,unsigned __int32 macID, stMsg_GetHostDetailsRsp * pOutputBuf)
	{
		return m_fn_st_getMACInfo(address,port,macID,pOutputBuf);
	}
	inline unsigned __int32  st_setHostDetails(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_SetHostDetailsReq * pInData,stMsg_SetHostDetailsRsp * pOutputBuf)
	{
		return m_fn_st_setHostDetails(address,port,macID,pInData,pOutputBuf);
	}
	inline unsigned __int32  st_removeDevice(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_RemoveDeviceReq * pInData,stMsg_RemoveDeviceRsp * pOutputBuf)
	{
		return m_fn_st_removeDevice(address,port,macID,pInData,pOutputBuf);
	}
	inline unsigned __int32  st_getDeviceList(const char * address, unsigned __int16 port,unsigned __int32 macID,stMsg_GetDeviceListRsp ** ppOutputBuf)
	{
		return m_fn_st_getDeviceList(address,port,macID,ppOutputBuf);
	}
	inline void  st_freeDeviceList(stMsg_GetDeviceListRsp * pOutputBuf)
	{
		return m_fn_st_freeDeviceList(pOutputBuf);
	}
	inline unsigned __int32 st_getDeviceParam(const char * address,unsigned __int16 port,unsigned __int32 macID,const stMsg_GetDeviceParamReq * pInBuf,stMsg_GetDeviceParamRsp ** ppOutputBuf)
	{
		return m_fn_st_getDeviceParam(address,port,macID,pInBuf,ppOutputBuf);
	}
	inline void st_freeDeviceParam(stMsg_GetDeviceParamRsp * pOutputBuf)
	{
		return  m_fn_st_freeDeviceParam(pOutputBuf);
	}
	inline unsigned __int32 st_setDeviceParam(const char * address,unsigned __int16 port,unsigned __int32 macID,const stMsg_setDeviceParamReq * pInData,stMsg_setDeviceParamRsp * pOutputBuf)
	{
		return m_fn_st_setDeviceParam(
			address,port,macID,pInData,pOutputBuf
			);
	}
	inline unsigned __int32 st_deviceCtrl(const char * address, unsigned __int16 port,unsigned __int32 macID, const stMsg_DeviceCtrlReq * pInData,const unsigned __int8 * pDAL,stMsg_DeviceCtrlRsp * pOutputBuf)
	{
		return m_fn_st_deviceCtrl(
			address,port,macID,pInData,pDAL,pOutputBuf
			);
	}

};