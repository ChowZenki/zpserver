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
		const PKLTS_Message * pRawMsg =
				(const PKLTS_Message *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_App_Layer * pAppLayer = &pRawMsg->trans_payload.app_layer;
		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_Trans_Header) - sizeof(PKLTS_App_Header);

		QString strSerial ;
		for (int i=0;i<nAppLen /*64*/ && pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i]!=0 ;++i)
		{
			strSerial+= pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i];
			m_serialNum.push_back((char)(pAppLayer->app_data.msg_HostRegistReq.HostSerialNum[i]));
		}

		//form return  Msgs
		quint16 nMsgLen = sizeof(PKLTS_App_Header)
				+sizeof(stMsg_HostRegistRsp);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
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
		const PKLTS_Message * pRawMsg =
				(const PKLTS_Message *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_App_Layer * pAppLayer = &pRawMsg->trans_payload.app_layer;

		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_Trans_Header)- sizeof(PKLTS_App_Header) - sizeof (quint32);
		QString strSerialNum ;
		quint32 UserID = pAppLayer->app_data.msg_HostLogonReq.ID;

		int nSwim = 0;
		while (  nSwim < 65 && nSwim <nAppLen && pAppLayer->app_data.msg_HostLogonReq.HostSerialNum[nSwim]!=0 )
			strSerialNum+= pAppLayer->app_data.msg_HostLogonReq.HostSerialNum[nSwim++];

		if (m_serialNum.length()<=0)
			m_serialNum = strSerialNum;

		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_App_Header)
				+sizeof(stMsg_HostLogonRsp);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
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
		quint16 nMsgLen = sizeof(PKLTS_App_Layer::tag_app_layer_header)
				+sizeof(stMsg_HostTimeCorrectRsp);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
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
		const PKLTS_Message * pRawMsg =
				(const PKLTS_Message *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_App_Layer * pAppLayer = &pRawMsg->trans_payload.app_layer;
		//How many devices
		unsigned int nItems = pAppLayer->app_data.msg_SendDeviceListReq.DeviceNums;
		//total string length
		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_Trans_Header)- sizeof(PKLTS_App_Header) - sizeof (quint16);
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
			++nSwim;
			strDeviceNames.push_back(strCurrentItem);
		}
		if ( strDeviceNames.size()<nItems)
		{
			qWarning()<<peerInfo()<<tr("Break Device List, There are  %1 devNames, other than %2.").arg(strDeviceNames.size()).arg(nItems)<< "\n";
			return false;
		}

		QVector<QString> strDeviceNos;
		while (  nSwim < nAppLen && strDeviceNos.size()<nItems)
		{
			QString strCurrentItem;
			while ( nSwim < nAppLen && ptr_start[nSwim]!=0 )
				strCurrentItem += ptr_start[nSwim++];
			++nSwim;
			strDeviceNos.push_back(strCurrentItem);
		}
		if ( strDeviceNos.size()<nItems)
		{
			qWarning()<<peerInfo()<<tr("Break Device List, There are  %1 devNos, other than %2.").arg(strDeviceNos.size()).arg(nItems)<< "\n";
			return false;
		}

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
		{
			qWarning()<<peerInfo()<<tr("Break Device List, There are  %1 devIDs, other than %2.").arg(strDeviceIDs.size()).arg(nItems)<< "\n";
			return false;
		}

		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_App_Header);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pApp->app_header.MsgType = 0x180B;

		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);
		bool res = dboper.insert_device_table(nItems,strDeviceNames,strDeviceNos,strDeviceIDs,this->uuid());
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return res;
	}
	bool st_clientNodeAppLayer::RecieveMacInfoFromHost()
	{
		const PKLTS_Message * pRawMsg =
				(const PKLTS_Message *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_App_Layer * pAppLayer = &pRawMsg->trans_payload.app_layer;
		//total string length
		int nAppLen = m_currentBlock.length()- sizeof(PKLTS_Trans_Header)- sizeof(PKLTS_App_Header) - sizeof (quint16);
		//firmwareVersion
		this->m_macInfo.FirmwareVersion = pAppLayer->app_data.msg_stMsg_SendMacInfoReq.FirmwareVersion;
		//the first byte of the string list
		const char * ptr_start =  pAppLayer->app_data.msg_stMsg_SendMacInfoReq.pStart ;
		//fill the HostName

		int nSwim = 0;
		int ct = 0;
		while ( nSwim < nAppLen && ptr_start[nSwim]!=0 && ct<65)
			this->m_macInfo.HostName[ct++] = ptr_start[nSwim++];
		this->m_macInfo.HostName[ct] = 0;
		++nSwim;

		ct = 0;
		while ( nSwim < nAppLen && ptr_start[nSwim]!=0 && ct<65)
			this->m_macInfo.HostInfo[ct++] = ptr_start[nSwim++];
		this->m_macInfo.HostInfo[ct] = 0;
		++nSwim;

		if (nSwim + sizeof (stMsg_SendMacInfoReq_internal::tag_TailData)<=nAppLen)
			memcpy(&m_macInfo.tail_data,ptr_start+nSwim, sizeof (stMsg_SendMacInfoReq_internal::tag_TailData));
		else
		{
			qWarning()<<peerInfo()<<tr("Break tag_TailData, We need  %1 bytes, larger than real data len %2.").arg(nSwim + sizeof (stMsg_SendMacInfoReq_internal::tag_TailData))
						.arg(nAppLen)<< "\n";
			return false;
		}

		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_App_Header);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pApp->app_header.MsgType = 0x180C;

		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);
		bool res = dboper.insert_mac_table(m_uuid,m_serialNum, m_macInfo);
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return res;
	}
	bool st_clientNodeAppLayer::RecieveEventFromHost()
	{
		const PKLTS_Message * pRawMsg =
				(const PKLTS_Message *)(
					((const char *)(m_currentBlock.constData()))
					);
		const PKLTS_App_Layer * pAppLayer = &pRawMsg->trans_payload.app_layer;


		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_App_Header)
				+sizeof(stMsg_EventPushRsp);
		QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
		char * ptr = array.data();
		PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
		PKLTS_App_Layer * pApp = &pMsg->trans_payload.app_layer;
		pMsg->trans_header.Mark = 0x55AA;
		pMsg->trans_header.SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );
		pMsg->trans_header.DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;
		pMsg->trans_header.DataLen = nMsgLen;
		pApp->app_header.MsgType = 0x4800;
		stMsg_EventPushRsp & reply = pApp->app_data.msg_stMsg_EventPushRsp;
		reply.DoneCode = 0;

		//Database Operations
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		st_operations dboper(&db);

		int nEvtID = pAppLayer->app_data.msg_stMsg_EventPushReq.event.EventType;
		switch (nEvtID)
		{
		//device joined
		case 0x0000:
			reply.DoneCode = dboper.add_new_device(this->uuid(), pAppLayer->app_data.msg_stMsg_EventPushReq.event.unEvent.evt_DeviceJoined.DeviceID);
			break;
			//device removed
		case 0x0001:
			reply.DoneCode = dboper.del_old_device(this->uuid(), pAppLayer->app_data.msg_stMsg_EventPushReq.event.unEvent.evt_DeviceRemoved.DeviceID);
			break;
			//Device Event
		case 0x0002:
		{
			size_t nOffsetDAL = sizeof(PKLTS_Trans_Header) + sizeof(PKLTS_App_Header) + sizeof(quint8) + sizeof(quint16);
			QByteArray array_Dal = m_currentBlock.mid(nOffsetDAL);
			reply.DoneCode = dboper.update_DAL_event(uuid(),array_Dal);
		}
			break;
		default:
			qWarning()<<peerInfo()<<tr("Unknown Event %1").arg(nEvtID)<< "\n";
			break;
		}
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return reply.DoneCode==0?true:false;
	}
}
