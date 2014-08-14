#include "st_clientnode_applayer.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <assert.h>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSettings>
#include <QSqlError>
#include <QSet>
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

		reply.DoneCode = 2;
		reply.ID = 0xffffffff;
		//strcpy(reply.TextInfo,"Unknown error");
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);

			QString sql = "select serialnum,macid,ifregisted from maclist where serialnum = ?;";
			query.prepare(sql);
			query.addBindValue(strSerial);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					int ncurrid = query.value(1).toInt(&bOk);
					int nregisdited =  query.value(2).toInt();
					if (bOk==true)
					{
						if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
						{
							reply.ID = ncurrid;
							reply.DoneCode = nregisdited==0?0:1;

							m_bUUIDRecieved = true;
							m_uuid = ncurrid;
							m_pClientTable->regisitClientUUID(this);
							if (nregisdited==0)
							{
								QSqlQuery queryUpdate(db);
								sql = "update maclist set ifregisted = 1 where  macid = ?;";
								queryUpdate.prepare(sql);
								queryUpdate.addBindValue(ncurrid);
								if (false==queryUpdate.exec())
								{
									reply.DoneCode = 2;
									//strcpy(reply.TextInfo,"Internal Server Error!");
								}
							}

						}
						else
						{
							//strcpy(reply.TextInfo,"Equip ID resource error.");
						}
					}
					//else
					//strcpy(reply.TextInfo,"Raw Dev ID Is Invalid.");
				}
				else
				{
					// No such device
					//strcpy(reply.TextInfo,"No such device ID.");
				}
			}
			else
			{
				//strcpy(reply.TextInfo,"Server Access Error.");
				emit evt_Message(this,tr("Database Access Error :")+query.lastError().text());
			}
		}
		else
		{
			//Server db is currently not accessable, wait.
			//strcpy(reply.TextInfo,"Server Not Accessable Now.");
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

		reply.DoneCode = 3;
		//strcpy(reply.TextInfo,"Unknown error");
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			QString sql = "select macid,serialnum from maclist where macid = ? and serialnum = ?;";
			query.prepare(sql);
			query.addBindValue(UserID);
			query.addBindValue(strSerialNum);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					quint32 ncurrid = query.value(0).toUInt(&bOk);
					if (bOk==true)
					{
						if (this->bIsValidUserId(ncurrid))
						{
							reply.DoneCode = 0;
							m_bLoggedIn = true;
							m_bUUIDRecieved = true;
							m_uuid = ncurrid;
							m_pClientTable->regisitClientUUID(this);
//							if (false == loadRelations())
//							{
//								reply.DoneCode = 3;
//							}
							//Cluster-Balance.
							//if (m_pClientTable->NeedRedirect(reply.Address_Redirect,&reply.port_Redirect))
							//{
							//	reply.DoneCode = 1;
							//}
						}
						// else
						// strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
					}
					//else
					// strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
				}
				else
				{
					// No such device
					//strcpy(reply.TextInfo,"No such user or password.");
				}
			}
			else
			{
				//strcpy(reply.TextInfo,"Server Access Error.");
				emit evt_Message(this,tr("Database Access Error :")+query.lastError().text());
			}
		}
		else
		{
			//Server db is currently not accessable, wait.
			//strcpy(reply.TextInfo,"Server Not Accessable Now.");
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

}
