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