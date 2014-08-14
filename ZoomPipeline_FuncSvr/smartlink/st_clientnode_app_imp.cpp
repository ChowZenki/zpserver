#include "st_clientnode_applayer.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <assert.h>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSettings>
#include <QSet>
#include <QVector>
#include "st_operations.h"
namespace ParkinglotsSvr{
	//0x0001 msg, stMsg_HostRegistReq
	bool st_clientNodeAppLayer::RegisitNewBoxNode()
	{
		const PKLTS_MSG * pRawMsg =
				(const PKLTS_MSG *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_APP_LAYER * pAppLayer = &pRawMsg->trans_payload.app_layer;
		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_TRANS_HEADER) - sizeof(PKLTS_APP_HEADER);

		QString strSerial ;
		for (int i=0;i<nAppLen /*64*/ && pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i]!=0 ;++i)
		{
			strSerial+= pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i];
			m_serialNum[i] =  pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i];
		}

		//form return  Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_HEADER)
				+sizeof(stMsg_HostRegistRsp);
		QByteArray array(sizeof(PKLTS_TRANS_HEADER) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_MSG * pMsg = (PKLTS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SerialNum = m_currentHeader.SerialNum;
		pMsg->trans_header.Priority = m_currentHeader.Priority;
		pMsg->trans_header.Reserved1 = 0;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pMsg->trans_header.Reserved2 = 0;

		pApp->app_header.AskID = m_current_app_header.AskID;
		pApp->app_header.MsgType = 0x1800;
		pApp->app_header.MsgFmtVersion = m_current_app_header.MsgFmtVersion;

		stMsg_HostRegistRsp & reply = pApp->app_data.msg_HostRegistRsp;

		//Check the database, find current equipment info
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);
		reply.DoneCode = 2;
		reply.ID = 0xffffffff;
		quint32 ncurrid = 0;
		reply.DoneCode = dboper.regisit_host(strSerial,&ncurrid);
		if (reply.DoneCode<2)
		{
			reply.ID = ncurrid;
			m_bUUIDRecieved = true;
			m_uuid = ncurrid;
			m_pClientTable->regisitClientUUID(this);
		}

		//Send back
		emit evt_SendDataToClient(this->sock(),array);



		return reply.DoneCode==2?false:true;
	}
	bool st_clientNodeAppLayer::LoginHost()
	{
		const PKLTS_MSG * pRawMsg =
				(const PKLTS_MSG *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_APP_LAYER * pAppLayer = &pRawMsg->trans_payload.app_layer;

		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_TRANS_HEADER)- sizeof(PKLTS_APP_HEADER) - sizeof (quint32);
		QString strSerialNum ;
		quint32 UserID = pAppLayer->app_data.msg_HostLogonReq.ID;

		int nSwim = 0;
		while (  nSwim < 65 && nSwim <nAppLen && pAppLayer->app_data.msg_HostLogonReq.HostSerialNum[nSwim]!=0 )
			strSerialNum+= pAppLayer->app_data.msg_HostLogonReq.HostSerialNum[nSwim++];


		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_HEADER)
				+sizeof(stMsg_HostLogonRsp);
		QByteArray array(sizeof(PKLTS_TRANS_HEADER) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_MSG * pMsg = (PKLTS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pApp->app_header.MsgType = 0x1801;
		stMsg_HostLogonRsp & reply = pApp->app_data.msg_HostLogonRsp;

		//Check the database, find current equipment info
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);
		reply.DoneCode = dboper.login_host(strSerialNum,UserID);
		if (reply.DoneCode==0)
		{
			m_bLoggedIn = true;
			m_bUUIDRecieved = true;
			m_uuid = UserID;
			m_pClientTable->regisitClientUUID(this);
		}
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return reply.DoneCode==3?false:true;
	}

	bool st_clientNodeAppLayer::Box2Svr_CorrectTime()
	{
		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_HostTimeCorrectRsp);
		QByteArray array(sizeof(PKLTS_TRANS_HEADER) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_MSG * pMsg = (PKLTS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SerialNum = m_currentHeader.SerialNum;
		pMsg->trans_header.Priority = m_currentHeader.Priority;
		pMsg->trans_header.Reserved1 = 0;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pMsg->trans_header.Reserved2 = 0;
		pApp->app_header.AskID = m_current_app_header.AskID;
		pApp->app_header.MsgType = 0x1802;
		pApp->app_header.MsgFmtVersion = m_current_app_header.MsgFmtVersion;

		stMsg_HostTimeCorrectRsp & reply = pApp->app_data.msg_HostTimeCorrectRsp;

		reply.DoneCode = 0;
		//reply.TextInfo[0]= 0;

		QDateTime dtm = QDateTime::currentDateTimeUtc();

		reply.DateTime.Year = dtm.date().year();
		reply.DateTime.Month = dtm.date().month();
		reply.DateTime.Day = dtm.date().day();
		reply.DateTime.Hour = dtm.time().hour();
		reply.DateTime.Minute = dtm.time().minute();
		reply.DateTime.Second = dtm.time().second();
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return reply.DoneCode==0?true:false;
	}
	bool st_clientNodeAppLayer::RecieveDeviceListFromHost()
	{
		const PKLTS_MSG * pRawMsg =
				(const PKLTS_MSG *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_APP_LAYER * pAppLayer = &pRawMsg->trans_payload.app_layer;
		//How many devices
		unsigned int nItems = pAppLayer->app_data.msg_SendDeviceListReq.DeviceNums;
		//total string length
		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_TRANS_HEADER)- sizeof(PKLTS_APP_HEADER) - sizeof (quint16);
		//the first byte of the string list
		const char * ptr_start =  pAppLayer->app_data.msg_SendDeviceListReq.pStrings ;

		QVector<QString> strDeviceNames;
		//fill the string list
		int nSwim = 0;
		while (  nSwim < nAppLen && strDeviceNames.size()<nItems)
		{
			QString strCurrentItem;
			while ( nSwim < nAppLen && ptr_start[nSwim]!=0 )
				strCurrentItem += ptr_start[nSwim++];
			strDeviceNames.push_back(strCurrentItem);
		}
		if ( strDeviceNames.size()<nItems)
			return false;

		QVector<QString> strDeviceNos;
		while (  nSwim < nAppLen && strDeviceNos.size()<nItems)
		{
			QString strCurrentItem;
			while ( nSwim < nAppLen && ptr_start[nSwim]!=0 )
				strCurrentItem += ptr_start[nSwim++];
			strDeviceNos.push_back(strCurrentItem);
		}
		if ( strDeviceNos.size()<nItems)
			return false;

		QVector<QString> strDeviceIDs;
		while (  nSwim < nAppLen  && strDeviceIDs.size()<nItems )
		{
			const char bufStrHex [] = "0123456789ABCDEF";
			QString strCurrentID;
			for (int i=0;i<24 && nSwim < nAppLen ;++i,++nSwim)
			{
				strCurrentID += bufStrHex[(ptr_start[nSwim]>>4)&0x0F];
				strCurrentID += bufStrHex[(ptr_start[nSwim])&0x0F];
			}
			if (strCurrentID.length()==48)
				strDeviceIDs.push_back(strCurrentID);
		}
		if ( strDeviceIDs.size()<nItems)
			return false;

		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_HEADER);
		QByteArray array(sizeof(PKLTS_TRANS_HEADER) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_MSG * pMsg = (PKLTS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pApp->app_header.MsgType = 0x180B;
		//Check the database, find current equipment info
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);
		bool res = dboper.insert_device_table(nItems,strDeviceNames,strDeviceNos,strDeviceIDs);
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return res;
	}

}
