#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "../libpklts_ctrl/st_ctrl.h"

using namespace ParkinglotsSvr;
void printMenu();

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port);
int main(int argc, char * argv [])
{
	pklts_ctrl ctrl(_T("libpklts_ctrl.dll")); 
	if (ctrl.valid()==false)
		return -1;

	char buffer_address[256],buffer_port[256];
	printf("Input server address:");
	gets_s(buffer_address);
	printf("Input server port:");
	gets_s(buffer_port);
	int nPort = atoi(buffer_port);
	
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
		default:
			break;
		};
	}while (Choice!=0);


	return 0;
}

void printMenu()
{
	printf ("1. st_getMACInfo\n");
	printf ("0. Exit\n");
	printf ("-----------------------------------------\n");
	printf ("Input method:");
}

void test_st_getMACInfo(pklts_ctrl * ctrl,const char * address, unsigned __int16 port)
{
	char inbuf[256];
	printf ("input MacID:");
	gets_s(inbuf);

	stMsg_GetHostDetailsRsp rsp;

	unsigned __int32 nMacID = atoi(inbuf);

	ctrl->st_getMACInfo(address,port,nMacID, &rsp);

}