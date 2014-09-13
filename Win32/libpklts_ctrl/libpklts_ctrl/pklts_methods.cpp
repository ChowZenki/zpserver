#include "stdafx.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "st_ctrlmsg.h"
int __cdecl RemoteFunctionCall(
	const char * address, 
	unsigned __int16 port,
	const unsigned __int8 * sendbuf,
	unsigned __int16 len,
	std::vector<unsigned __int8> & vec_result
	) ;

unsigned __int32 getUniqueSrcID();

using namespace ParkinglotsSvr;

extern "C"  unsigned __int32 __stdcall st_getMACInfo(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	stMsg_GetHostDetailsRsp * pOutputBuf)
{
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header) /* + sizeof (msg_GetHostDetailsRsp)*/;
	unsigned char * messageSend = new unsigned char [nSendLen];

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x2000;
	
	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x2800)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->DoneCode) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->DoneCode),sizeof(pOutputBuf->DoneCode),pSwim+nCurrStart,sizeof(pOutputBuf->DoneCode));
							nCurrStart += sizeof(pOutputBuf->DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//HostType
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->HostType) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->HostType),sizeof(pOutputBuf->HostType),pSwim+nCurrStart,sizeof(pOutputBuf->HostType));
							nCurrStart += sizeof(pOutputBuf->HostType);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//FirmwareVersion
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->FirmwareVersion) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->FirmwareVersion),sizeof(pOutputBuf->FirmwareVersion),pSwim+nCurrStart,sizeof(pOutputBuf->FirmwareVersion));
							nCurrStart += sizeof(pOutputBuf->FirmwareVersion);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//HostName
					if (nRes == ALL_SUCCEED)
					{
						int nCpStr = 0;
						while ( nCurrStart < nTotalLen && nCpStr <_countof(pOutputBuf->HostName) )
						{
							pOutputBuf->HostName[nCpStr++] = pSwim[nCurrStart];
							if (pSwim[nCurrStart]==0)
								break;
							++nCurrStart;
						}
						if (nCpStr == 0)
							nRes = ERRTRANS_LESS_DATA;
						else
							++nCurrStart;
					}
					//HostInfo
					if (nRes == ALL_SUCCEED)
					{
						int nCpStr = 0;
						while ( nCurrStart < nTotalLen && nCpStr <_countof(pOutputBuf->HostInfo) )
						{
							pOutputBuf->HostInfo[nCpStr++] = pSwim[nCurrStart];
							if (pSwim[nCurrStart]==0)
								break;
							++nCurrStart;
						}
						if (nCpStr == 0)
							nRes = ERRTRANS_LESS_DATA;
						else
							++nCurrStart;
					}
					//ConnetType
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->ConnetType) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->ConnetType),sizeof(pOutputBuf->ConnetType),pSwim+nCurrStart,sizeof(pOutputBuf->ConnetType));
							nCurrStart += sizeof(pOutputBuf->ConnetType);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//IEEEAdd
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->IEEEAdd) < nTotalLen)
						{
							memcpy_s(pOutputBuf->IEEEAdd,sizeof(pOutputBuf->IEEEAdd),pSwim+nCurrStart,sizeof(pOutputBuf->IEEEAdd));
							nCurrStart += sizeof(pOutputBuf->IEEEAdd);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//IEEEAdd_Flag
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->IEEEAdd_Flag) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->IEEEAdd_Flag),sizeof(pOutputBuf->IEEEAdd_Flag),pSwim+nCurrStart,sizeof(pOutputBuf->IEEEAdd_Flag));
							nCurrStart += sizeof(pOutputBuf->IEEEAdd_Flag);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//PANID
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->PANID) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->PANID),sizeof(pOutputBuf->PANID),pSwim+nCurrStart,sizeof(pOutputBuf->PANID));
							nCurrStart += sizeof(pOutputBuf->PANID);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//PANID_Flag
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->PANID_Flag) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->PANID_Flag),sizeof(pOutputBuf->PANID_Flag),pSwim+nCurrStart,sizeof(pOutputBuf->PANID_Flag));
							nCurrStart += sizeof(pOutputBuf->PANID_Flag);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//PANID_Flag
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->EPANID) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->EPANID),sizeof(pOutputBuf->EPANID),pSwim+nCurrStart,sizeof(pOutputBuf->EPANID));
							nCurrStart += sizeof(pOutputBuf->EPANID);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//PANID_Flag
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->EPANID_Flag) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->EPANID_Flag),sizeof(pOutputBuf->EPANID_Flag),pSwim+nCurrStart,sizeof(pOutputBuf->EPANID_Flag));
							nCurrStart += sizeof(pOutputBuf->EPANID_Flag);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//SensorNum
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->SensorNum) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->SensorNum),sizeof(pOutputBuf->SensorNum),pSwim+nCurrStart,sizeof(pOutputBuf->SensorNum));
							nCurrStart += sizeof(pOutputBuf->SensorNum);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}

					//RelayNum
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->RelayNum) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->RelayNum),sizeof(pOutputBuf->RelayNum),pSwim+nCurrStart,sizeof(pOutputBuf->RelayNum));
							nCurrStart += sizeof(pOutputBuf->RelayNum);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//ANSensorNum
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->ANSensorNum) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->ANSensorNum),sizeof(pOutputBuf->ANSensorNum),pSwim+nCurrStart,sizeof(pOutputBuf->ANSensorNum));
							nCurrStart += sizeof(pOutputBuf->ANSensorNum);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//ANRelayNum
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->ANRelayNum) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->ANRelayNum),sizeof(pOutputBuf->ANRelayNum),pSwim+nCurrStart,sizeof(pOutputBuf->ANRelayNum));
							nCurrStart += sizeof(pOutputBuf->ANRelayNum);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	return nRes;
}

extern "C"  unsigned __int32 __stdcall st_setHostDetails(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	const stMsg_SetHostDetailsReq * pInData,
	stMsg_SetHostDetailsRsp * pOutputBuf)
{
	//Calc the string length
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header)+ sizeof (stMsg_SetHostDetailsReq);
	unsigned char * messageSend = new unsigned char [nSendLen+2];
	nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header);

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x2001;

	for (int i=0;i<sizeof(pInData->HostName) && pInData->HostName[i]!=0;++i)
	{
		++pMessageSend->trans_header.DataLen;
		messageSend[nSendLen++] = pInData->HostName[i];
	}
	messageSend[nSendLen++] = 0;
	++pMessageSend->trans_header.DataLen;
	for (int i=0;i<sizeof(pInData->HostInfo) && pInData->HostInfo[i]!=0;++i)
	{
		messageSend[nSendLen++] = pInData->HostName[i];
		++pMessageSend->trans_header.DataLen;
	}
	messageSend[nSendLen++] = 0;
	++pMessageSend->trans_header.DataLen;

	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x2801)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->DoneCode) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->DoneCode),sizeof(pOutputBuf->DoneCode),pSwim+nCurrStart,sizeof(pOutputBuf->DoneCode));
							nCurrStart += sizeof(pOutputBuf->DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}			

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	return nRes;
}

extern "C"  unsigned __int32 __stdcall st_removeDevice(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	const stMsg_RemoveDeviceReq * pInData,
	stMsg_RemoveDeviceRsp * pOutputBuf)
{
	//Calc the string length
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header)+ sizeof (stMsg_RemoveDeviceReq);
	unsigned char * messageSend = new unsigned char [nSendLen];

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header) +  sizeof (stMsg_RemoveDeviceReq);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x200A;

	memcpy_s(
		&(pMessageSend->trans_payload.app_layer.app_data.msg_RemoveDeviceReq),
		sizeof(pMessageSend->trans_payload.app_layer.app_data.msg_RemoveDeviceReq),
		pInData,
		sizeof(pMessageSend->trans_payload.app_layer.app_data.msg_RemoveDeviceReq)
		);
	
	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x280A)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->DoneCode) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->DoneCode),sizeof(pOutputBuf->DoneCode),pSwim+nCurrStart,sizeof(pOutputBuf->DoneCode));
							nCurrStart += sizeof(pOutputBuf->DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}			

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	return nRes;
}

extern "C"  unsigned __int32 __stdcall st_getDeviceList(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	stMsg_GetDeviceListRsp ** ppOutputBuf)
{
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header);
	unsigned char * messageSend = new unsigned char [nSendLen];

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x200B;
	
	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	stMsg_GetDeviceListRsp * resu = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x280B)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					unsigned __int8 DoneCode = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(DoneCode) < nTotalLen)
						{
							memcpy_s(&DoneCode,sizeof(DoneCode),pSwim+nCurrStart,sizeof(DoneCode));
							nCurrStart += sizeof(DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//Count
					unsigned __int16 nCount = 0;
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(nCount) < nTotalLen)
						{
							memcpy_s(&nCount,sizeof(nCount),pSwim+nCurrStart,sizeof(nCount));
							nCurrStart += sizeof(nCount);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
						if (nCount==0)
							nRes = ERRTRANS_LESS_DATA;
					}
					//Allocate Memory

					if (nRes == ALL_SUCCEED)
					{
						size_t AllocLen = sizeof(stMsg_GetDeviceListRsp) + sizeof (stMsg_GetDeviceListRsp::stCall_DeviceNode) * (nCount - 1);
						resu = (stMsg_GetDeviceListRsp * ) new char [AllocLen]; 
						memset(resu,0,AllocLen);
						resu->DoneCode = DoneCode;
						resu->nDevCount = nCount;
						int nCheck[3] = {0,0,0};
						for (unsigned __int16 i=0;
							i<nCount 
							&& nCurrStart < nTotalLen 
							;++i,++nCheck[0])
						{
							int currSp = 0;
							do
							{
								resu->devicetable[i].DeviceName[currSp++] = pSwim[nCurrStart];
								if (pSwim[nCurrStart]==0)
									break;
								++nCurrStart;
							}while (nCurrStart < nTotalLen  && currSp < sizeof(resu->devicetable[0].DeviceName));
							++nCurrStart;
						}
						for (unsigned __int16 i=0;
							i<nCount 
							&& nCurrStart < nTotalLen 
							;++i,++nCheck[1])
						{
							int currSp = 0;
							do
							{
								resu->devicetable[i].No[currSp++] = pSwim[nCurrStart];
								if (pSwim[nCurrStart]==0)
									break;
								++nCurrStart;
							}while (nCurrStart < nTotalLen  && currSp < sizeof(resu->devicetable[0].No));
							++nCurrStart;
						}
						for (unsigned __int16 i=0;
							i<nCount 
							&& nCurrStart < nTotalLen 
							;++i,++nCheck[2])
						{
							for (size_t j = 0;j<sizeof(resu->devicetable[0].DeviceID)&& nCurrStart < nTotalLen ;++j)
								resu->devicetable[i].DeviceID[j] = pSwim[nCurrStart++];
						}
						if (nCurrStart < nTotalLen)
							nRes = ERRTRANS_ERROR_DATA;
						if (nCheck[0] < nCount || nCheck[1] < nCount ||nCheck[2] < nCount )
							nRes = ERRTRANS_LESS_DATA;
					}
					

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	if (nRes == ALL_SUCCEED)
		*ppOutputBuf = resu;
	else if (resu)
	{
		char * ptr = (char *) resu;
		delete [] ptr;
	}
	return nRes;
}

extern "C"  void __stdcall st_freeDeviceList(
	stMsg_GetDeviceListRsp * pOutputBuf)
{
		char * ptr = (char *) pOutputBuf;
		delete [] ptr;	
}

extern "C"  unsigned __int32 __stdcall st_getDeviceParam(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	const stMsg_GetDeviceParamReq * pInBuf,
	stMsg_GetDeviceParamRsp ** ppOutputBuf)
{
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header) + sizeof(stMsg_GetDeviceParamReq);
	unsigned char * messageSend = new unsigned char [nSendLen];

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header) + sizeof(stMsg_GetDeviceParamReq);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x200C;
	memcpy_s(&(pMessageSend->trans_payload.app_layer.app_data.msg_GetDeviceParamReq),
		sizeof(stMsg_GetDeviceParamReq),
		pInBuf,
		sizeof(stMsg_GetDeviceParamReq));
	
	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	stMsg_GetDeviceParamRsp * resu = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x280C)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					unsigned __int8 DoneCode = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(DoneCode) < nTotalLen)
						{
							memcpy_s(&DoneCode,sizeof(DoneCode),pSwim+nCurrStart,sizeof(DoneCode));
							nCurrStart += sizeof(DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//Device ID
					unsigned __int8 DevID [24];
					memset(DevID,0,sizeof(DevID));
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(DevID) < nTotalLen)
						{
							memcpy_s(DevID,sizeof(DevID),pSwim+nCurrStart,sizeof(DevID));
							nCurrStart += sizeof(DevID);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					unsigned __int8  Opt_DeviceName = 0;
					//Opt_DeviceName
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(Opt_DeviceName) < nTotalLen)
						{
							memcpy_s(& Opt_DeviceName,sizeof( Opt_DeviceName),pSwim+nCurrStart,sizeof( Opt_DeviceName));
							nCurrStart += sizeof( Opt_DeviceName);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					unsigned __int8  Opt_DeviceInfo = 0;
					//Opt_DeviceInfo
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(Opt_DeviceInfo) < nTotalLen)
						{
							memcpy_s(& Opt_DeviceInfo,sizeof( Opt_DeviceInfo),pSwim+nCurrStart,sizeof( Opt_DeviceInfo));
							nCurrStart += sizeof( Opt_DeviceInfo);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					unsigned __int8  Opt_DALStatus = 0;
					//Opt_DALStatus
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(Opt_DALStatus) < nTotalLen)
						{
							memcpy_s(& Opt_DALStatus,sizeof( Opt_DALStatus),pSwim+nCurrStart,sizeof( Opt_DALStatus));
							nCurrStart += sizeof( Opt_DALStatus);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}

					//Dev Name
					char DeviceName[32];
					memset(DeviceName,0,sizeof(DeviceName));
					if (Opt_DeviceName && nRes == ALL_SUCCEED)
					{
						if (nCurrStart < nTotalLen)
						{
							int currSp = 0;
							do
							{
								DeviceName[currSp++] = pSwim[nCurrStart];
								if (pSwim[nCurrStart]==0)
									break;
								++nCurrStart;
							}while (nCurrStart < nTotalLen  && currSp < sizeof(DeviceName));
							++nCurrStart;
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}

					//Dev Info
					char DeviceInfo[64];
					memset(DeviceInfo,0,sizeof(DeviceInfo));
					if (Opt_DeviceInfo&& nRes == ALL_SUCCEED)
					{
						if (nCurrStart < nTotalLen)
						{
							int currSp = 0;
							do
							{
								DeviceInfo[currSp++] = pSwim[nCurrStart];
								if (pSwim[nCurrStart]==0)
									break;
								++nCurrStart;
							}while (nCurrStart < nTotalLen  && currSp < sizeof(DeviceInfo));
							++nCurrStart;
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					//DAL Buf
					int nDALBytes = 1;
					unsigned __int8 * DALBuf = 0;
					if (Opt_DALStatus&& nRes == ALL_SUCCEED)
					{
						if (nCurrStart < nTotalLen)
						{
							nDALBytes = nTotalLen - nCurrStart;
							DALBuf = new unsigned __int8 [nDALBytes];
							memcpy_s(DALBuf,nDALBytes,pSwim+nCurrStart,nDALBytes);
							nCurrStart += nDALBytes;
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}
					if (nCurrStart < nTotalLen)
						nRes = ERRTRANS_ERROR_DATA;

					if (nRes == ALL_SUCCEED)
					{
						size_t AllocLen = sizeof(stMsg_GetDeviceParamRsp) + nDALBytes - 1;
						resu = (stMsg_GetDeviceParamRsp * ) new char [AllocLen]; 
						memset(resu,0,AllocLen);
						resu->DoneCode = DoneCode;
						resu->Opt_DeviceName = Opt_DeviceName;
						resu->Opt_DeviceInfo = Opt_DeviceInfo;
						resu->Opt_DALStatus = Opt_DALStatus;
						memcpy_s(resu->DeviceName,sizeof(resu->DeviceName),DeviceName,sizeof(	DeviceName));			
						memcpy_s(resu->DeviceInfo,sizeof(resu->DeviceInfo),DeviceInfo,sizeof(	DeviceInfo));
						if (DALBuf)
						{
							resu->DALStatusBytesLen = nDALBytes;
							memcpy_s(resu->DALStatusBytes,nDALBytes,DALBuf,nDALBytes);
							delete [] DALBuf;
							DALBuf = 0;
						}
						else
							resu->DALStatusBytesLen = 0;						
					}
					if (DALBuf)
					{
						delete [] DALBuf;
							DALBuf = 0;
					}

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	if (nRes == ALL_SUCCEED)
		*ppOutputBuf = resu;
	else if (resu)
	{
		char * ptr = (char *) resu;
		delete [] ptr;
	}
	return nRes;
}
extern "C"  void __stdcall st_freeDeviceParam(
	stMsg_GetDeviceParamRsp * pOutputBuf)
{
		char * ptr = (char *) pOutputBuf;
		delete [] ptr;	
}

extern "C"  unsigned __int32 __stdcall st_setDeviceParam(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	const stMsg_setDeviceParamReq * pInData,
	stMsg_setDeviceParamRsp * pOutputBuf)
{
	//Calc the string length
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header)+ sizeof (stMsg_setDeviceParamReq);
	unsigned char * messageSend = new unsigned char [nSendLen+2];
	nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header);

	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x200D;

	pMessageSend->trans_payload.app_layer.app_data.msg_setDeviceParamReq.Opt_DeviceInfo = pInData->Opt_DeviceInfo;
	++nSendLen;++pMessageSend->trans_header.DataLen;
	pMessageSend->trans_payload.app_layer.app_data.msg_setDeviceParamReq.Opt_DeviceName = pInData->Opt_DeviceName;
	++nSendLen;++pMessageSend->trans_header.DataLen;

	if (pInData->Opt_DeviceName)
	{
		for (int i=0;i<sizeof(pInData->DeviceName) && pInData->DeviceName[i]!=0;++i)
		{
			++pMessageSend->trans_header.DataLen;
			messageSend[nSendLen++] = pInData->DeviceName[i];
		}
		messageSend[nSendLen++] = 0;
		++pMessageSend->trans_header.DataLen;
	}
	
	if (pInData->Opt_DeviceInfo)
	{
		for (int i=0;i<sizeof(pInData->DeviceInfo) && pInData->DeviceInfo[i]!=0;++i)
		{
			++pMessageSend->trans_header.DataLen;
			messageSend[nSendLen++] = pInData->DeviceInfo[i];
		}
		messageSend[nSendLen++] = 0;
		++pMessageSend->trans_header.DataLen;
	}

	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x280D)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->DoneCode) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->DoneCode),sizeof(pOutputBuf->DoneCode),pSwim+nCurrStart,sizeof(pOutputBuf->DoneCode));
							nCurrStart += sizeof(pOutputBuf->DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}			

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	return nRes;
}


extern "C"  unsigned __int32 __stdcall st_deviceCtrl(
	const char * address, 
	unsigned __int16 port,
	unsigned __int32 macID, 
	const stMsg_DeviceCtrlReq * pInData,
	const unsigned __int8 * pDAL,
	stMsg_DeviceCtrlRsp * pOutputBuf)
{
	//Calc the string length
	int nSendLen = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header)+ sizeof (pInData->DeviceID) + pInData->DALArrayLength;
	unsigned char * messageSend = new unsigned char [nSendLen];
	PKLTS_Message * pMessageSend = (PKLTS_Message *) messageSend;
	pMessageSend->trans_header.Mark = 0x55AA;
	pMessageSend->trans_header.SrcID = (unsigned __int32)((unsigned __int64)(getUniqueSrcID()) & 0xffffffff );
	pMessageSend->trans_header.DstID = (unsigned __int32)((unsigned __int64)(macID) & 0xffffffff );;
	pMessageSend->trans_header.DataLen =  sizeof(PKLTS_App_Header);
	pMessageSend->trans_payload.app_layer.app_header.MsgType = 0x200E;
	for (int i=0;i<24;++i)
		pMessageSend->trans_payload.app_layer.app_data.msg[i] = pInData->DeviceID[i];
	for (int i=0;i<pInData->DALArrayLength;++i)
		pMessageSend->trans_payload.app_layer.app_data.msg[i+24] = pDAL[i];

	std::vector<unsigned __int8> vec_response;
	int nRes = RemoteFunctionCall(address,port,
		messageSend,nSendLen,
		vec_response
		);
	delete [] messageSend;
	messageSend = 0;
	//Dealing with result
	if (nRes==ALL_SUCCEED )
	{
		if ( vec_response.size()>=sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header))
		{
			PKLTS_Message * pMessageSend = (PKLTS_Message *) vec_response.data();
			if (pMessageSend->trans_header.Mark!=0x55AA)
				nRes = ERRTRANS_ERROR_MARK;
			else
			{
				if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x280E)
				{
					unsigned char * pSwim =(unsigned char *) &(pMessageSend->trans_payload.app_layer.app_data);
					size_t nTotalLen = vec_response.size() - sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);
					size_t nCurrStart = 0;
					//Done Code
					if (nRes == ALL_SUCCEED)
					{
						if ( nCurrStart - 1 + sizeof(pOutputBuf->DoneCode) < nTotalLen)
						{
							memcpy_s(&(pOutputBuf->DoneCode),sizeof(pOutputBuf->DoneCode),pSwim+nCurrStart,sizeof(pOutputBuf->DoneCode));
							nCurrStart += sizeof(pOutputBuf->DoneCode);
						}
						else
							nRes = ERRTRANS_LESS_DATA;
					}			

				}
				else if (pMessageSend->trans_payload.app_layer.app_header.MsgType == 0x0000)
					nRes = ERRTRANS_DST_NOT_REACHABLE;
				else
					nRes = ERRTRANS_ERROR_MSG_TYPE;
			}
		}
		else
			nRes = ERRTRANS_LESS_DATA;			
	}
	return nRes;
}