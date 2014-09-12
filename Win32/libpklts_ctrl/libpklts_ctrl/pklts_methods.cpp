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

