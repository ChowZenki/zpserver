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

	if (nRes==0)
	{
	}


	delete [] messageSend;
	messageSend = 0;
	return nRes;
}