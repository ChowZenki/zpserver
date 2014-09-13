#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "st_ctrloader.h"

using namespace ParkinglotsSvr;
void printMenu();

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_setHostDetails(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_removeDevice(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_getDeviceList(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_getDeviceParam(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_setDeviceParam(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_deviceCtrl(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);

int main(int argc, char * argv [])
{
	//First, Load the DLL interface.
	pklts_ctrl ctrl(_T("libpklts_ctrl.dll")); 
	if (ctrl.valid()==false)
		return -1;
	//Second, We will get server address from user.
	char buffer_address[256],buffer_port[256];
	printf("Input server address:");
	gets_s(buffer_address);
	printf("Input server port:");
	gets_s(buffer_port);
	int nPort = atoi(buffer_port);
	
	//Third, We start a simple CUI, let's test it
	int Choice = 0;
	do{
		printMenu();
		char bufChoice[16];
		gets_s(bufChoice);
		Choice = atoi(bufChoice);
		switch (Choice)
		{
		case 1:
			test_st_getMACInfo(&ctrl,buffer_address,nPort);
			break;
		case 2:
			test_st_setHostDetails(&ctrl,buffer_address,nPort);
			break;
		case 3:
			test_st_removeDevice(&ctrl,buffer_address,nPort);
			break;
		case 4:
			test_st_getDeviceList(&ctrl,buffer_address,nPort);
			break;		
		case 5:
			test_st_getDeviceParam(&ctrl,buffer_address,nPort);
			break;
		case 6:
			test_st_setDeviceParam(&ctrl,buffer_address,nPort);
			break;
		case 7:
			test_st_deviceCtrl(&ctrl,buffer_address,nPort);
			break;
		default:
			break;
		};
	}while (Choice!=0);


	return 0;
}

void printMenu()
{
	printf ("\n\n-----------------------------------------\n");
	printf ("1. st_getMACInfo\n");
	printf ("2. st_setHostDetails\n");
	printf ("3. st_removeDevice\n");
	printf ("4. st_getDeviceList\n");
	printf ("5. st_getDeviceParam\n");
	printf ("6. st_setDeviceParam\n");
	printf ("7. st_deviceCtrl\n");
	printf ("0. Exit\n");
	printf ("-----------------------------------------\n");
	printf ("Input method:");
}

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	//Then, define a structure, to hold result.
	stMsg_GetHostDetailsRsp rsp;

	unsigned __int32 nMacID = atoi(inbuf);

	//And then, Call the method directly, just like a native method.
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_getMACInfo(address,port,nMacID, &rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp.DoneCode);
		printf ("rsp.HostType = %d\n",(unsigned int)rsp.HostType);
		printf ("rsp.FirmwareVersion = %d\n",(unsigned int)rsp.FirmwareVersion);
		printf ("rsp.HostName = %s\n",rsp.HostName);
		printf ("rsp.HostInfo = %s\n",rsp.HostInfo);
		printf ("rsp.ConnetType = %d\n",(unsigned int)rsp.ConnetType);
		printf ("...\n");
		printf ("rsp.SensorNum = %d\n",(unsigned int)rsp.SensorNum);
		printf ("rsp.RelayNum = %d\n",(unsigned int)rsp.RelayNum);
		printf ("rsp.ANSensorNum = %d\n",(unsigned int)rsp.ANSensorNum);
		printf ("rsp.ANRelayNum = %d\n",(unsigned int)rsp.ANRelayNum);
	}

}

void test_st_setHostDetails(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	//Then, define a structure, to hold result.
	stMsg_SetHostDetailsReq DataIn;
	stMsg_SetHostDetailsRsp rsp;
	printf ("input HostName:");
	gets_s(DataIn.HostName);
	printf ("input HostInfo:");
	gets_s(DataIn.HostInfo);

	unsigned __int32 nMacID = atoi(inbuf);

	//And then, Call the method directly, just like a native method.
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_setHostDetails(address,port,nMacID, &DataIn, &rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp.DoneCode);
	}

}

void test_st_removeDevice(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{

	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	//Then, define a structure, to hold result.
	stMsg_RemoveDeviceReq DataIn;
	stMsg_RemoveDeviceRsp rsp;
	printf ("input DEVID(24 HEX Bytes):");
	char buf[256];
	gets_s(buf);
	for (int i=0;i<24;++i)
	{
		unsigned __int8 cv = 0;
		if (buf[i*2] >='0' &&  buf[i*2] <='9')	cv += buf[i*2]-'0';
		else if (buf[i*2] >='a' &&  buf[i*2] <='f') cv += buf[i*2]-'a' + 10;
		else if (buf[i*2] >='A' &&  buf[i*2] <='F') cv += buf[i*2]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		cv <<= 4;
		if (buf[i*2+1] >='0' &&  buf[i*2+1] <='9')	cv += buf[i*2+1]-'0';
		else if (buf[i*2+1] >='a' &&  buf[i*2+1] <='f') cv += buf[i*2+1]-'a' + 10;
		else if (buf[i*2+1] >='A' &&  buf[i*2+1] <='F') cv += buf[i*2+1]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		DataIn.DeviceID[i] = cv;
	}
	unsigned __int32 nMacID = atoi(inbuf);

	//And then, Call the method directly, just like a native method.
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_removeDevice(address,port,nMacID, &DataIn, &rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp.DoneCode);
	}

}

void test_st_getDeviceList(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);
	unsigned __int32 nMacID = atoi(inbuf);	
	//Then, define a structure, to hold result.
	stMsg_GetDeviceListRsp * rsp = 0;
	//And then, Call the method directly, just like a native method.
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_getDeviceList(address,port,nMacID, &rsp);
	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp->DoneCode);
		printf ("rsp.nDevCount = %d\n",(unsigned int)rsp->nDevCount);
		for (int i=0;i<rsp->nDevCount;++i)
		{
			printf ("DeviceName[%5d] = %s\n",i,rsp->devicetable[i].DeviceName);
			printf ("DeviceNO  [%5d] = %s\n",i,rsp->devicetable[i].No);
			printf ("DeviceID  [%5d] = ",i);
			for (int j=0;j<24;++j)	printf ("%02x",rsp->devicetable[i].DeviceID[j]);
			printf ("\n");
		}
	}
	if (rsp)
		ctrl->st_freeDeviceList(rsp);
}

void test_st_getDeviceParam(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
		//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	stMsg_GetDeviceParamReq req;
	printf ("input DEVID(24 HEX Bytes):");
	char buf[256];
	gets_s(buf);
	for (int i=0;i<24;++i)
	{
		unsigned __int8 cv = 0;
		if (buf[i*2] >='0' &&  buf[i*2] <='9')	cv += buf[i*2]-'0';
		else if (buf[i*2] >='a' &&  buf[i*2] <='f') cv += buf[i*2]-'a' + 10;
		else if (buf[i*2] >='A' &&  buf[i*2] <='F') cv += buf[i*2]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		cv <<= 4;
		if (buf[i*2+1] >='0' &&  buf[i*2+1] <='9')	cv += buf[i*2+1]-'0';
		else if (buf[i*2+1] >='a' &&  buf[i*2+1] <='f') cv += buf[i*2+1]-'a' + 10;
		else if (buf[i*2+1] >='A' &&  buf[i*2+1] <='F') cv += buf[i*2+1]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		req.DeviceID[i] = cv;
	}
	req.Opt_DALStatus = 1;
	req.Opt_DeviceInfo = 1;
	req.Opt_DeviceName = 1;

	unsigned __int32 nMacID = atoi(inbuf);

	//Then, define a structure, to hold result.
	stMsg_GetDeviceParamRsp  * rsp;	//And then, Call the method directly, just like a native method.
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_getDeviceParam(address,port,nMacID, &req,&rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp->DoneCode);
		if (rsp->Opt_DeviceName) printf ("rsp.DeviceName = %s\n",rsp->DeviceName);
		if (rsp->Opt_DeviceInfo) printf ("rsp.DeviceName = %s\n",rsp->DeviceInfo);
		if (rsp->Opt_DALStatus)
		{
			printf ("rsp.DALLen = %d\n",(unsigned int)rsp->DALStatusBytesLen);
			for (size_t i=0;i<(unsigned int)rsp->DALStatusBytesLen;++i)
				printf ("%02X",rsp->DALStatusBytes[i]);
			printf ("\n");
		}
	}
	ctrl->st_freeDeviceParam(rsp);

}
void test_st_setDeviceParam(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	stMsg_setDeviceParamReq req;

	printf ("input DEVID(24 HEX Bytes):");
	char buf[256];
	gets_s(buf);
	for (int i=0;i<24;++i)
	{
		unsigned __int8 cv = 0;
		if (buf[i*2] >='0' &&  buf[i*2] <='9')	cv += buf[i*2]-'0';
		else if (buf[i*2] >='a' &&  buf[i*2] <='f') cv += buf[i*2]-'a' + 10;
		else if (buf[i*2] >='A' &&  buf[i*2] <='F') cv += buf[i*2]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		cv <<= 4;
		if (buf[i*2+1] >='0' &&  buf[i*2+1] <='9')	cv += buf[i*2+1]-'0';
		else if (buf[i*2+1] >='a' &&  buf[i*2+1] <='f') cv += buf[i*2+1]-'a' + 10;
		else if (buf[i*2+1] >='A' &&  buf[i*2+1] <='F') cv += buf[i*2+1]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		req.DeviceID[i] = cv;
	}
	
	printf ("Input Device Name:");
	gets_s(req.DeviceName);
	printf ("Input Device Info:");
	gets_s(req.DeviceInfo);
	req.Opt_DeviceInfo = 1;
	req.Opt_DeviceName = 1;

	unsigned __int32 nMacID = atoi(inbuf);

	stMsg_setDeviceParamRsp rsp;
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_setDeviceParam(address,port,nMacID, &req,&rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp.DoneCode);
	}
}
void test_st_deviceCtrl(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	stMsg_DeviceCtrlReq req;

	printf ("input DEVID(24 HEX Bytes, NO spaces):");
	char buf[256];
	gets_s(buf);
	for (int i=0;i<24;++i)
	{
		unsigned __int8 cv = 0;
		if (buf[i*2] >='0' &&  buf[i*2] <='9')	cv += buf[i*2]-'0';
		else if (buf[i*2] >='a' &&  buf[i*2] <='f') cv += buf[i*2]-'a' + 10;
		else if (buf[i*2] >='A' &&  buf[i*2] <='F') cv += buf[i*2]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		cv <<= 4;
		if (buf[i*2+1] >='0' &&  buf[i*2+1] <='9')	cv += buf[i*2+1]-'0';
		else if (buf[i*2+1] >='a' &&  buf[i*2+1] <='f') cv += buf[i*2+1]-'a' + 10;
		else if (buf[i*2+1] >='A' &&  buf[i*2+1] <='F') cv += buf[i*2+1]-'A' + 10;
		else {printf ("Error Reading Hex Data! "); return;};
		req.DeviceID[i] = cv;
	}
	
	std::vector <unsigned char> vec_DAL;
	printf ("input DAL HEX Bytes, NO spaces:");
	char *buffer_dal = new char [65536];
	gets_s(buffer_dal,65536);
	unsigned __int16 nSwim = 0;
	while (buffer_dal[nSwim] && nSwim < 65536)
	{
		unsigned __int8 cv = 0;
		if (buffer_dal[nSwim] >='0' &&  buffer_dal[nSwim] <='9')	cv += buffer_dal[nSwim]-'0';
		else if (buffer_dal[nSwim] >='a' &&  buffer_dal[nSwim] <='f') cv += buffer_dal[nSwim]-'a' + 10;
		else if (buffer_dal[nSwim] >='A' &&  buffer_dal[nSwim] <='F') cv += buffer_dal[nSwim]-'A' + 10;
		else break;
		cv <<= 4;
		++nSwim;
		if (buffer_dal[nSwim] >='0' &&  buffer_dal[nSwim] <='9')	cv += buffer_dal[nSwim]-'0';
		else if (buffer_dal[nSwim] >='a' &&  buffer_dal[nSwim] <='f') cv += buffer_dal[nSwim]-'a' + 10;
		else if (buffer_dal[nSwim] >='A' &&  buffer_dal[nSwim] <='F') cv += buffer_dal[nSwim]-'A' + 10;
		else break;
		vec_DAL.push_back(cv);
	}
	req.DALArrayLength = vec_DAL.size();

	unsigned __int32 nMacID = atoi(inbuf);

	stMsg_DeviceCtrlRsp rsp;
	//Inside the function, a remote call will be executed.
	int res = ctrl->st_deviceCtrl(address,port,nMacID, &req,vec_DAL.data(), &rsp);

	//Check the result, and print the result.
	printf ("Res = %d\n",res);
	if (res == ALL_SUCCEED)
	{
		printf ("rsp.DoneCode = %d\n",(unsigned int)rsp.DoneCode);
	}	
}