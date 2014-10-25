//gsoap ns service name:        PKLTSCtrlService
//gsoap ns service style:       rpc
//gsoap ns service encoding:    encoded
//gsoap ns service namespace:   http://127.0.0.1/PKLTSCtrlService.wsdl
//gsoap ns service location:    http://127.0.0.1:8080
//gsoap ns schema namespace:    urn:PKLTSCtrlService

	enum ns__ErrMessage{
		ALL_SUCCEED          = 0,
		//Net Errors, 4Bit
		ERRNET_WSAStartupFailed = 1,
		ERRNET_GetAddrInfoError = 2,
		ERRNET_SocketFailedErr  = 3,
		ERRNET_ConnectionFailed = 4,
		ERRNET_SendDataFailed   = 5,
		ERRNET_RecvDataFailed   = 6,
		ERRNET_ShutDownFailed   = 7,
		//TransLayer Errors, 4Bit
		ERRTRANS_ERROR_MARK				 =0x10,
		ERRTRANS_LESS_DATA				 =0x11,
		ERRTRANS_DST_NOT_REACHABLE		 =0x12,
		ERRTRANS_ERROR_MSG_TYPE			 =0x13,
		ERRTRANS_ERROR_DATA              =0x14
	};

	//0x2000
	struct ns__GetHostDetailsReq{
		unsigned int macID;
	};

	//0x2800
	struct ns__GetHostDetailsRsp{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
		unsigned short HostType;
		unsigned short FirmwareVersion;
		char HostName[64];
		char HostInfo[64];
		unsigned char ConnetType;
		//1£ºGPRS£¬2£º3G£¬3£ºWAN£¬4£ºLAN
		unsigned char IEEEAdd[8];
		unsigned char IEEEAdd_Flag;
		unsigned char PANID[2];
		unsigned char PANID_Flag;
		unsigned char EPANID[8];
		unsigned char EPANID_Flag;
		unsigned short SensorNum;
		unsigned short RelayNum;
		unsigned short ANSensorNum;
		unsigned short ANRelayNum;
	};
	//0x2001
	struct ns__SetHostDetailsReq
	{
		unsigned int macID;
		char HostName[64];
		char HostInfo[64];
	};
	//0x2801
	struct ns__SetHostDetailsRsp{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
	};
	//0x200A
	struct ns__RemoveDeviceReq {
		unsigned int macID;
		unsigned char DeviceID[24];
	};
	//0x280A
	struct ns__RemoveDeviceRsp{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
	};
	//0x200B
	struct ns__GetDeviceListReq{
		unsigned int macID;
	};
	//0x280B
	struct   ns__stCall_DeviceNode
	{
		char DeviceName [32];
		char No[64];
		unsigned char DeviceID[24];
	};
	struct ns__DeviceNodeArray
	{
		int __size;
		struct ns__stCall_DeviceNode ** __ptr;
	};
	struct ns__GetDeviceListRsp
	{
		enum ns__ErrMessage retCode;
		unsigned char   DoneCode;
		unsigned short  nDevCount;
		struct ns__DeviceNodeArray array;
	};
	//0x200C
	struct ns__GetDeviceParamReq
	{
		unsigned int macID;
		unsigned char DeviceID[24];
		unsigned char Opt_DeviceName;
		unsigned char Opt_DeviceInfo;
		unsigned char Opt_DALStatus;
	};
	//0x280C
	struct ns__DeviceDAL
	{
		int __size;
		unsigned char ** __ptr;
	};
	struct ns__GetDeviceParamRsp
	{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
		unsigned char DeviceID[24];
		unsigned char Opt_DeviceName;
		unsigned char Opt_DeviceInfo;
		unsigned char Opt_DALStatus;
		char DeviceName[32];
		char DeviceInfo[64];
		struct ns__DeviceDAL DAL;
	};

	//0x200D
	struct ns__setDeviceParamReq
	{
		unsigned int macID;
		unsigned char DeviceID[24];
		unsigned char Opt_DeviceName;
		unsigned char Opt_DeviceInfo;
		char DeviceName[32];
		char DeviceInfo[64];
	};
	//0x280D
	struct ns__setDeviceParamRsp
	{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
	};
	//0x280E

	struct ns__DeviceCtrlReq
	{
		unsigned int macID;
		unsigned char DeviceID[24];
		struct ns__DeviceDAL DAL;
	};
	//0x280E
	struct ns__DeviceCtrlRsp
	{
		enum ns__ErrMessage retCode;
		unsigned char DoneCode;
	};
	
	int ns__getMACInfo(			 
		 struct ns__GetHostDetailsReq * pInData,
		struct ns__GetHostDetailsRsp * pOutputBuf
		);
	
	int ns__setHostDetails(		
		 struct ns__SetHostDetailsReq * pInData,
		struct ns__SetHostDetailsRsp * pOutputBuf
		);
	
	int ns__removeDevice(
		 struct ns__RemoveDeviceReq * pInData,
		struct ns__RemoveDeviceRsp * pOutputBuf
		);
	
	int ns__getDeviceList(
		 struct ns__GetDeviceListReq * pInData,
		struct ns__GetDeviceListRsp * ppOutputBuf);
		
	int ns__getDeviceParam(	
		 struct ns__GetDeviceParamReq * pInBuf,
		struct ns__GetDeviceParamRsp * ppOutputBuf
		);	
	
	int ns__setDeviceParam(
		 struct ns__setDeviceParamReq * pInData,
		struct ns__setDeviceParamRsp * pOutputBuf);
		
	int ns__deviceCtrl(	
		 struct ns__DeviceCtrlReq * pInData,
		struct ns__DeviceCtrlRsp * pOutputBuf
		);
