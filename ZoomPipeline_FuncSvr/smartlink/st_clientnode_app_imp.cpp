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
		const PKLTS_APP_LAYER * pAppLayer =
				(const PKLTS_APP_LAYER *)(
					((const char *)(m_currentBlock.constData()))
					+sizeof(PKLTS_TRANS_MSG)-1);
		int nAppLen = m_currentBlock.length()- (sizeof(PKLTS_TRANS_MSG)-1) - sizeof(tag_pklts_app_layer::tag_app_layer_header);

		QString strSerial ;
		for (int i=0;i<nAppLen /*64*/ && pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i]!=0 ;++i)
		{
			strSerial+= pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i];
			m_serialNum[i] =  pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i];
		}

		//form return  Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_HostRegistRsp);
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		pMsg->SerialNum = m_currentHeader.SerialNum;
		pMsg->Priority = m_currentHeader.Priority;
		pMsg->Reserved1 = 0;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;
		pMsg->Reserved2 = 0;


		pApp->header.AskID = m_current_app_header.header.AskID;
		pApp->header.MsgType = 0x1800;
		pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

		stMsg_HostRegistRsp & reply = pApp->MsgUnion.msg_HostRegistRsp;

		//Check the database, find current equipment info
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

		reply.DoneCode = 2;
		reply.ID = 0xffffffff;
		//strcpy(reply.TextInfo,"Unknown error");
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);

			QString sql = "select host_serial_num,equip_id,first_login from instruments where host_serial_num = ?;";
			query.prepare(sql);
			query.addBindValue(strSerial);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					int ncurrid = query.value(1).toInt(&bOk);
					int nfirstlogin =  query.value(2).toInt();
					if (bOk==true)
					{
						if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
						{
							reply.ID = ncurrid;
							reply.DoneCode = nfirstlogin==1?0:1;
							//strcpy(reply.TextInfo,"Re-regisit Succeed.");
							m_bUUIDRecieved = true;
							m_uuid = ncurrid;
							m_pClientTable->regisitClientUUID(this);
							if (nfirstlogin==1)
							{
								//strcpy(reply.TextInfo,"First-Regisit Succeed.");
								QSqlQuery queryUpdate(db);
								sql = "update instruments set first_login = 0 where  host_serial_num = ?;";
								queryUpdate.prepare(sql);
								queryUpdate.addBindValue(strSerial);
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
	bool st_clientNodeAppLayer::LoginClient()
	{
		const PKLTS_APP_LAYER * pAppLayer =
				(const PKLTS_APP_LAYER *)(
					((const char *)(m_currentBlock.constData()))
					+sizeof(PKLTS_TRANS_MSG)-1);
		int nAppLen = m_currentBlock.length()- (sizeof(PKLTS_TRANS_MSG)-1)- sizeof(tag_pklts_app_layer::tag_app_layer_header) - sizeof (quint32);
		QString strPasswd ;
		quint32 UserID = pAppLayer->MsgUnion.msg_HostLogonReq.ID;

		int nSwim = 0;
		while (  nSwim < 65 && nSwim <nAppLen && pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[nSwim]!=0 )
			strPasswd+= pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[nSwim++];


		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_HostLogonRsp);
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;

		pApp->header.MsgType = 0x7FFE;

		stMsg_HostLogonRsp & reply = pApp->MsgUnion.msg_HostLogonRsp;

		//Check the database, find current equipment info
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

		reply.DoneCode = 3;
		//strcpy(reply.TextInfo,"Unknown error");
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);

			QString sql = "select user_id,password from users where user_id = ? and password = ?;";
			query.prepare(sql);
			query.addBindValue(UserID);
			query.addBindValue(strPasswd);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					quint32 ncurrid = query.value(1).toUInt(&bOk);
					if (bOk==true)
					{
						if (this->bIsValidUserId(ncurrid))
						{

							reply.DoneCode = 0;
							//reply.UserID = ncurrid;
							m_bLoggedIn = true;
							m_bUUIDRecieved = true;
							m_uuid = ncurrid;
							m_pClientTable->regisitClientUUID(this);
							if (false == loadRelations())
							{
								reply.DoneCode = 3;
							}
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
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		//pMsg->version = m_currentHeader.version;
		pMsg->SerialNum = m_currentHeader.SerialNum;
		pMsg->Priority = m_currentHeader.Priority;
		pMsg->Reserved1 = 0;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;
		pMsg->Reserved2 = 0;


		pApp->header.AskID = m_current_app_header.header.AskID;
		pApp->header.MsgType = 0x1802;
		pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

		stMsg_HostTimeCorrectRsp & reply = pApp->MsgUnion.msg_HostTimeCorrectRsp;

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

	bool st_clientNodeAppLayer::Box2Svr_UploadUserTable()
	{
		const PKLTS_APP_LAYER * pAppLayer =
				(const PKLTS_APP_LAYER *)(
					((const char *)(m_currentBlock.constData()))
					+sizeof(PKLTS_TRANS_MSG)-1);

		if (m_currentMessageSize!=sizeof(PKLTS_TRANS_MSG) - 1
				+ sizeof (PKLTS_APP_LAYER::tag_app_layer_header)
				+ sizeof (stMsg_UploadUserListReq) - sizeof(quint32)
				+ sizeof (quint32) * (pAppLayer->MsgUnion.msg_UploadUserListReq.UserNum)
				)
		{
			emit evt_Message(this,tr("Broken Message stMsg_UploadUserListReq, size not correct."));
			return false;
		}

		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_UploadUserListRsp);
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;

		pApp->header.MsgType = 0x7FFC;

		stMsg_UploadUserListRsp & reply = pApp->MsgUnion.msg_UploadUserListRsp;

		reply.DoneCode = 1;
		//strcpy(reply.TextInfo,"Unknown error");
		if (loadRelations()==true )
		{
			for (quint32 i = 0; i <pAppLayer->MsgUnion.msg_UploadUserListReq.UserNum && i<4096;i++)
			{
				if (bIsValidUserId(pAppLayer->MsgUnion.msg_UploadUserListReq.pUserIDList[i])==false)
					continue;
				m_matched_nodes.insert(pAppLayer->MsgUnion.msg_UploadUserListReq.pUserIDList[i]);
			}
			if (true == saveRelations())
			{
				reply.DoneCode = 0;
				//strcpy(reply.TextInfo,"Succeeded.");
			}
		}
		else
		{
			//Server db is currently not accessable, wait.
			//strcpy(reply.TextInfo,"Failed to load current relations from db.");
		}


		//Send back
		emit evt_SendDataToClient(this->sock(),array);



		return reply.DoneCode==0?true:false;
	}
	bool st_clientNodeAppLayer::Box2Svr_DownloadUserTable()
	{
		bool res = true;
		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_DownloadUserListRsp) - sizeof(quint32);
		int nSz = 0;
		if (loadRelations()==true )
		{
			nSz = m_matched_nodes.size();
			nMsgLen += nSz * sizeof(quint32);
		}
		else
			res = false;

		//form Msgs
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;

		pApp->header.MsgType = 0x7FFB;

		stMsg_DownloadUserListRsp & reply = pApp->MsgUnion.msg_DownloadUserListRsp;

		reply.DoneCode = res==true?0:1;
		//if (res==false)
		//strcpy(reply.TextInfo,"load Relation failed");
		int ii = 0;
		foreach (quint32 it, m_matched_nodes)
		{
			if (ii < nSz && ii<32768)
			{
				reply.pUserIDList[ii] = it;
				reply.UserNum = (quint16) ii+1;
			}
			++ii;
		}


		//Send back
		emit evt_SendDataToClient(this->sock(),array);



		return reply.DoneCode==0?true:false;
	}
	bool st_clientNodeAppLayer::ClientLogout()
	{
		bool res = true;
		//form Msgs
		quint16 nMsgLen = sizeof(PKLTS_APP_LAYER::tag_app_layer_header)
				+sizeof(stMsg_ClientLogoutRsp);
		//int nSz = 0;
		//form Msgs
		QByteArray array(sizeof(PKLTS_TRANS_MSG) + nMsgLen - 1,0);
		char * ptr = array.data();
		PKLTS_TRANS_MSG * pMsg = (PKLTS_TRANS_MSG *)ptr;
		PKLTS_APP_LAYER * pApp = (PKLTS_APP_LAYER *)(((unsigned char *)
														  (ptr))+sizeof(PKLTS_TRANS_MSG)-1
														 );
		pMsg->Mark = 0x55AA;
		pMsg->SrcID = (quint32)((quint64)(m_currentHeader.DstID) & 0xffffffff );

		pMsg->DstID = (quint32)((quint64)(m_currentHeader.SrcID) & 0xffffffff );;

		pMsg->DataLen = nMsgLen;


		pApp->header.MsgType = 0x7FFD;

		stMsg_ClientLogoutRsp & reply = pApp->MsgUnion.msg_ClientLogoutRsp;

		reply.DoneCode = res==true?0:1;
		//if (res==false)
		//strcpy(reply.TextInfo,"load Relation failed");
		//Send back
		emit evt_SendDataToClient(this->sock(),array);
		return reply.DoneCode==0?true:false;
	}
}
