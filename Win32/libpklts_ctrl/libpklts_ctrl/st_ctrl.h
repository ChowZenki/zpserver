#ifndef ST_CTRL_H
#define ST_CTRL_H
#include <windows.h>
#include <tchar.h>
#pragma  pack (push,1)
namespace ParkinglotsSvr{
	//0x2000
	struct stMsg_GetHostDetailsReq{
		//目前为空
	};

	//0x2800
	struct stMsg_GetHostDetailsRsp{
		unsigned __int8 DoneCode;//执行结果代码
		unsigned __int16 HostType;//网关类型
		unsigned __int16 FirmwareVersion;//网关固件版本号
		char HostName[64];//网关名称，长度不超过64字节
		char HostInfo[64];//网关描述，长度不超过64字节
		unsigned __int8 ConnetType;// 与数据中心连接方式：
		//1：GPRS，2：3G，3：WAN，4：LAN
		unsigned __int8 IEEEAdd[8];// 协调器IEEE地址，8字节十六进制数
		unsigned __int8 IEEEAdd_Flag;// 协调器IEEE地址使能：0未使能，1使能
		unsigned __int8 PANID[2];// PANID，2字节十六进制数
		unsigned __int8 PANID_Flag;// PANID使能：0未使能，1使能
		unsigned __int8 EPANID[8];// 扩展PANID，8字节十六进制数
		unsigned __int8 EPANID_Flag;// 扩展PANID使能：0未使能，1使能
		unsigned __int16 SensorNum;// 传感器数量
		unsigned __int16 RelayNum;// 中继数量
		unsigned __int16 ANSensorNum;// 异常传感器数量
		unsigned __int16 ANRelayNum;// 异常中继数量
	};

}
#pragma  pack (pop)

typedef unsigned __int32 (__stdcall * fp_st_getMACInfo)(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	ParkinglotsSvr::stMsg_GetHostDetailsRsp * pOutputBuf);

//This class help client app to get dll method easily
class pklts_ctrl{
private:
	HMODULE m_dllMod;
	fp_st_getMACInfo m_fn_st_getMACInfo;
public:
	inline pklts_ctrl(const _TCHAR * dllFilePath)
	{
		m_dllMod = ::LoadLibrary(dllFilePath);
		if (m_dllMod !=NULL)
		{
			m_fn_st_getMACInfo = (fp_st_getMACInfo )::GetProcAddress(m_dllMod,"st_getMACInfo");
		}
		else
		{
			m_fn_st_getMACInfo = NULL;
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
		return true;
	}

public:
	inline unsigned __int32  st_getMACInfo(const char * address, unsigned __int16 port,unsigned __int32 macID, ParkinglotsSvr::stMsg_GetHostDetailsRsp * pOutputBuf)
	{
		return m_fn_st_getMACInfo(address,port,macID,pOutputBuf);
	}

};


#endif