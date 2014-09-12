#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "../libpklts_ctrl/st_ctrl.h"

using namespace ParkinglotsSvr;
void printMenu();

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
void test_st_setHostDetails(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
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
	printf ("0. Exit\n");
	printf ("-----------------------------------------\n");
	printf ("Input method:");
}

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	//This function test st_getMACInfo.

	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	//Then, define a stMsg_GetHostDetailsRsp structure, to hold result.
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
	//This function test st_getMACInfo.

	//First, Get The Mac ID you want to ask.
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	//Then, define a stMsg_GetHostDetailsRsp structure, to hold result.
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