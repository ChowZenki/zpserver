#include "st_clientnode_applayer.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
namespace SmartLink{
	st_clientNodeAppLayer::st_clientNodeAppLayer(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
		st_clientNode_baseTrans(pClientTable,pClientSock,parent)
	{

		m_bLoggedIn= false;
		memset(&m_current_app_header,0,sizeof(SMARTLINK_MSG_APP));
		memset(m_serialNum,0,sizeof(m_serialNum));
		memset(m_username,0,sizeof(m_username));

	}

	bool st_clientNodeAppLayer::loadRelations()
	{
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		if (db.isValid()==true && db.isOpen()==true )
		{
			QString sql = "select ";
			if (m_serialNum[0]!=0)
				sql += "user_id from relations where equip_id = ?;";
			else if (m_username[0]!=0)
				sql += "equip_id from relations where user_id = ?;";
			else
			{
				emit evt_Message(tr("try to save relations before login!"));
				return false;
			}
			QSqlQuery query(db);
			query.prepare(sql);
			query.addBindValue((quint32)m_uuid);
			if (false== query.exec())
			{
				emit evt_Message(tr("try to get relations Failed! ")+ query.lastError().text());
				return false;
			}

			m_matched_nodes.clear();
			while (query.next())
			{
				quint32 val = query.value(0).toUInt();
				m_matched_nodes.insert(val);
			}
			return true;
		}
		else
		{
			//Server db is currently not accessable, wait.
			emit evt_Message("Server Not Accessable Now.");
		}
		return false;
	}

	bool st_clientNodeAppLayer::saveRelations()
	{
		QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
		if (db.isValid()==true && db.isOpen()==true )
		{
			QString sql = "delete from relations where ";
			if (m_serialNum[0]!=0)
			{
				QSqlQuery query(db);
				sql += "equip_id = ?;";
				query.prepare(sql);
				query.addBindValue((quint32)m_uuid);
				if (false== query.exec())
				{
					emit evt_Message(tr("try to del old relations Failed! ")+ query.lastError().text());
					return false;
				}
			}
			else if (m_username[0]!=0)
			{
				QSqlQuery query(db);
				sql += "user_id = ?;";
				query.prepare(sql);
				query.addBindValue((quint32)m_uuid);
				if (false== query.exec())
				{
					emit evt_Message(tr("try to del old relations Failed! ")+ query.lastError().text());
					return false;
				}
			}
			else
			{
				emit evt_Message(tr("try to save relations before login!"));
				return false;
			}

			sql = "insert into relations (equip_id,user_id) values (?,?);";
			//the forigen key can automatic avoid non-existing values.
			foreach(quint32 nodeid,m_matched_nodes)
			{

				QSqlQuery query(db);
				if (m_serialNum[0]!=0)
				{
					if (bIsValidUserId(nodeid)==false)
						continue;
					query.prepare(sql);
					query.addBindValue((quint32)m_uuid);
					query.addBindValue(nodeid);

				}
				else if (m_username[0]!=0)
				{
					if (bIsValidEquipId(nodeid)==false)
						continue;
					query.prepare(sql);
					query.addBindValue(nodeid);
					query.addBindValue((quint32)m_uuid);
				}
				if (false== query.exec())
				{
					emit evt_Message(tr("try to insert new relations Failed! ")+ query.lastError().text());
					return false;
				}
			}
			return true;
		}
		else
		{
			//Server db is currently not accessable, wait.
			emit evt_Message("Server Not Accessable Now.");
		}
		return false;
	}

	//!deal current message
	int st_clientNodeAppLayer::deal_current_message_block()
	{
		//then , Start deal to-server messages
		//Server - deal messages
		emit evt_Message("Debug:" + m_currentBlock.toHex());
		if (m_currentHeader.destin_id==0x00000001)
		{
			if (this->m_bLoggedIn==false || this->m_bUUIDRecieved==false)
			{
				//To-Server Messages does not wait for message-block completes
				if (false==Deal_ToServer_Handshakes())
				{
					m_currentBlock = QByteArray();
					emit evt_Message(tr("To-server Message Failed."));
					emit evt_close_client(this->sock());
				}
			}
			else
			{
				if (bIsValidEquipId(m_currentHeader.source_id))
				{
					//Deal Box->Svr Msgs
					if (false==Deal_Node2Svr_Msgs())
					{
						m_currentBlock = QByteArray();
						emit evt_Message(tr("Box To Server Message Failed."));
						emit evt_close_client(this->sock());
					}
				}
				else if (bIsValidUserId(m_currentHeader.source_id) )
				{
					//Deal Client->Svr Msgs
					if (false==Deal_Node2Svr_Msgs())
					{
						m_currentBlock = QByteArray();
						emit evt_Message(tr("Client To Server Message Failed."));
						emit evt_close_client(this->sock());
					}
				}
				else if (m_currentHeader.source_id==0xFFFFFFFF)
				{
					m_currentBlock = QByteArray();
					emit evt_Message(tr("warning, UUID 0xFFFFFFFF.ignore"));

				}
				else
				{
					m_currentBlock = QByteArray();
					emit evt_Message(tr("Bad UUID %1. Client Kicked out").arg(m_currentHeader.source_id));
					emit evt_close_client(this->sock());
				}
			}

		}
		//deal Broadcast messages
		else if (m_currentHeader.destin_id==0xFFFFFFFC)
		{
			//need furture works.
			//Do Nothing
			emit evt_Message(tr("Broadcast Message is not currently supported."));
			m_currentBlock = QByteArray();
		}
		else if (m_currentHeader.destin_id==0xFFFFFFFD)
		{
			//need furture works.
			//Do Nothing
			emit evt_Message(tr("Broadcast Message is not currently supported."));
			m_currentBlock = QByteArray();
		}
		else
		{
			//find Destin Client using Hash.
			st_clientNode_baseTrans * destin_node = m_pClientTable->clientNodeFromUUID(m_currentHeader.destin_id);
			if (destin_node==NULL)
			{
				//need further dev, insert into db, or catched on disk.
				//destin client is un-reachable, or in another function server.
				//need server-to-server channels to re-post this message.
				emit evt_Message(tr("Destin ID ") + QString("%1").arg(m_currentHeader.destin_id) + tr(" is not currently logged in."));

				//Do Nothing
			}
			else
			{
				emit evt_SendDataToClient(destin_node->sock(),m_currentBlock);
				m_currentBlock = QByteArray();

			}

		}
		if (bytesLeft()==0)
			m_current_app_header.header.MsgType = 0x00;
		return     st_clientNode_baseTrans::deal_current_message_block();
	}

	//deal to-server handshakes
	bool st_clientNodeAppLayer::Deal_ToServer_Handshakes()
	{
		bool res = true;
		//qDebug()<<m_currentHeader.data_length<<"\n";
		//qDebug()<<this->m_currentBlock.toHex()<<"\n";
		if (m_currentHeader.data_length < sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
			return false;
		if (m_currentMessageSize < sizeof(SMARTLINK_MSG) - 1 + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
		{
			// header is not complete, return
			return true;
		}
		//Catch the header
		if (m_current_app_header.header.MsgType==0x00)
			memcpy((void *)&this->m_current_app_header,
				   ((unsigned char *)this->m_currentBlock.constData()) + sizeof(SMARTLINK_MSG) - 1,
				   sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
				   );
		if (m_current_app_header.header.MsgFmtVersion!=0x01)
		{
			emit evt_Message(tr("Application Layer Version too new."));
			emit evt_close_client(this->sock());
			return false;
		}
		//do
		qDebug()<<m_current_app_header.header.MsgType<<"\n";
		switch (m_current_app_header.header.MsgType)
		{
		case 0x1000:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize>
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_HostRegistReq)+64)
			{
				emit evt_Message(tr("Broken Message stMsg_HostRegistReq, size not correct."));
				res = false;
			}
			else
				res = this->RegisitNewBoxNode();
			break;
		case 0x1001:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize>
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_HostLogonReq)+64)
			{
				emit evt_Message(tr("Broken Message stMsg_HostLogonReq, size not correct."));
				res = false;
			}
			else
				res = this->LoginBox();
			break;
		case 0x3000:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize>
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_ClientLoginReq)+66)
			{
				emit evt_Message(tr("Broken Message stMsg_ClientLoginReq, size not correct."));
				res = false;
			}
			else
				res = this->LoginClient();
			break;
		default:
			emit evt_Message(tr("Message type not supported."));
			res = false;
			break;
		}

		m_currentBlock.clear();


		return res;
	}

	//Deal Box2Svr Msgs
	bool st_clientNodeAppLayer::Deal_Node2Svr_Msgs()
	{
		bool res = true;

		if (m_currentHeader.data_length < sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
			return false;
		if (m_currentMessageSize < sizeof(SMARTLINK_MSG) - 1 + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
		{
			// header is not complete, return
			return true;
		}
		//Catch the header
		if (m_current_app_header.header.MsgType==0x00)
			memcpy((void *)&this->m_current_app_header,
				   ((unsigned char *)this->m_currentBlock.constData()) + sizeof(SMARTLINK_MSG) - 1,
				   sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
				   );
		if (m_current_app_header.header.MsgFmtVersion!=0x01)
		{
			emit evt_Message(tr("Application Layer Version too new."));
			emit evt_close_client(this->sock());
			return false;
		}
		//do
		switch (m_current_app_header.header.MsgType)
		{
		case 0x1002:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize!=
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_HostTimeCorrectReq))
			{
				emit evt_Message(tr("Broken Message stMsg_HostRegistReq, size not correct."));
				res = false;
			}
			else
				res = this->Box2Svr_CorrectTime();
			break;
		case 0x1003:
			if (bytesLeft()>0)
				return true;

			if (m_currentMessageSize<
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_UploadUserListReq) - sizeof(quint32))
			{
				emit evt_Message(tr("Broken Message stMsg_UploadUserListReq, size not correct."));
				res = false;
			}
			else
				res = this->Box2Svr_UploadUserTable();
			break;
		case 0x1004:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize!=
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_DownloadUserListReq))
			{
				emit evt_Message(tr("Broken Message stMsg_DownloadUserListReq, size not correct."));
				res = false;
			}
			else
				res = this->Box2Svr_DownloadUserTable();
			break;
		case 0x3001:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize!=
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_ClientLogoutReq))
			{
				emit evt_Message(tr("Broken Message stMsg_ClientLogoutReq, size not correct."));
				res = false;
			}
			else
				res = this->ClientLogout();
			break;
		case 0x3002:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			if (m_currentMessageSize!=
					sizeof(SMARTLINK_MSG) - 1
					+ sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
					+ sizeof (stMsg_GetHostListReq))
			{
				emit evt_Message(tr("Broken Message stMsg_GetHostListReq, size not correct."));
				res = false;
			}
			else
				res = this->GetHostList();
			break;
		default:
			emit evt_Message(tr("Message type not supported."));
			res = false;
			break;
		}

		m_currentBlock.clear();


		return res;
	}

}
