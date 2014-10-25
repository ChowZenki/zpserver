#include <stdio.h>
#include "PKLTSCtrlService.nsmap"
#include "../libpklts_ctrl/st_ctrl.h"
#include "libpklts_ctrl_loader.h"
int main(int argc, char ** argv)
{
	pklts_ctrl ctrl(_T("libpklts_ctrl.dll")); 
	if (ctrl.valid()==false)
		return -1;
	int nPort = 8080;

	struct soap fun_soap;
	soap_init(&fun_soap);
	int nMaster = (int)soap_bind(&fun_soap, NULL, nPort, 100);

	if (nMaster < 0)
	{
		soap_print_fault(&fun_soap, stderr);
		exit(-1);
	}

	fprintf(stderr, "Socket connection successful : master socket = %d\n", nMaster);

	while (true)
	{
		int nSlave = (int)soap_accept(&fun_soap);
		if (nSlave < 0)
		{
			soap_print_fault(&fun_soap, stderr);
			exit(-1);
		}
		fprintf(stderr, "Socket connection successful : slave socket = %d\n", nSlave);
		soap_serve(&fun_soap);
		soap_end(&fun_soap);
	}
	return 0;
}

int ns__deviceCtrl(struct soap *,struct ns__DeviceCtrlReq *,struct ns__DeviceCtrlRsp *)
{
	return 0;
}

int ns__getDeviceList(struct soap *,struct ns__GetDeviceListReq *,struct ns__GetDeviceListRsp *)
{
	return 0;
}

int  ns__removeDevice(struct soap *,struct ns__RemoveDeviceReq *,struct ns__RemoveDeviceRsp *)
{
	return 0;
}

int ns__setDeviceParam(struct soap *,struct ns__setDeviceParamReq *,struct ns__setDeviceParamRsp *)
{
	return 0;
}

int ns__getDeviceParam(struct soap *,struct ns__GetDeviceParamReq *,struct ns__GetDeviceParamRsp *)
{
	return 0;
}

int ns__getMACInfo(struct soap *,struct ns__GetHostDetailsReq *,struct ns__GetHostDetailsRsp *)
{
	return 0;
}

int ns__setHostDetails(struct soap *,struct ns__SetHostDetailsReq *,struct ns__SetHostDetailsRsp *)
{
	return 0;
}