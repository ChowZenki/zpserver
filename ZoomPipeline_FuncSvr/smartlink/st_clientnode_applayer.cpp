#include "st_clientnode_applayer.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
namespace ParkinglotsSvr{
	st_clientNodeAppLayer::st_clientNodeAppLayer(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
		st_clientNode_baseTrans(pClientTable,pClientSock,parent)
	{

		m_bLoggedIn= false;
		memset(&m_current_app_header,0,sizeof(PKLTS_App_Header));

	}

	//!deal current message
	int st_clientNodeAppLayer::deal_current_message_block()
	{
		if (st_clientNode_baseTrans::deal_current_message_block()!=0)
			return 1;
		//then , Start deal to-server messages
		if (m_currentHeader.DstID==0x00000001)
		{
			if (this->m_bLoggedIn==false || this->m_bUUIDRecieved==false)
			{
				//To-Server Messages does not wait for message-block completes
				if (false==Deal_ToServer_Handshakes())
				{
					qWarning()<<this->peerInfo()<<tr(" Deal_ToServer_Handshakes() failed.");
					m_currentBlock = QByteArray();
					emit evt_Message(this,tr("To-server Message Failed."));
					emit evt_close_client(this->sock());
				}
			}
			else
			{
				if (bIsValidUserId(m_currentHeader.SrcID) )
				{
					//Deal Client->Svr Msgs
					if (false==Deal_Node2Svr_Msgs())
					{
						qWarning()<<this->peerInfo()<<tr(" Deal_Node2Svr_Msgs() failed.");
						m_currentBlock = QByteArray();
						emit evt_Message(this,tr("Client To Server Message Failed."));
						emit evt_close_client(this->sock());
					}
				}
				else if (m_currentHeader.SrcID==0xFFFFFFFF)
				{
					qWarning()<<this->peerInfo()<<tr(" warning, UUID 0xFFFFFFFF.ignore.");
					m_currentBlock = QByteArray();
					emit evt_Message(this,tr("warning, UUID 0xFFFFFFFF.ignore"));

				}
				else
				{
					m_currentBlock = QByteArray();
					qWarning()<<this->peerInfo()<<tr("Bad UUID %1. Client Kicked out").arg(m_currentHeader.SrcID);
					emit evt_Message(this,tr("Bad UUID %1. Client Kicked out").arg(m_currentHeader.SrcID));
					emit evt_close_client(this->sock());
				}
			}

		}
		else
		{
			//find Destin Client using Hash.
			st_clientNode_baseTrans * destin_node = m_pClientTable->clientNodeFromUUID(m_currentHeader.DstID);
			if (destin_node==NULL)
			{
				//need server-to-server channels to re-post this message.
				QString svr = m_pClientTable->cross_svr_find_uuid(m_currentHeader.DstID);
				if (svr.length()<=0)
				{
					qWarning()<<this->peerInfo()<<
								tr("Destin ID ") + QString("%1").arg(m_currentHeader.DstID) + tr(" is not currently logged in.");
					emit evt_Message(this,tr("Destin ID ") + QString("%1").arg(m_currentHeader.DstID) + tr(" is not currently logged in."));
				}
				else
					m_pClientTable->cross_svr_send_data(svr,m_currentBlock);


			}
			else
			{
				emit evt_SendDataToClient(destin_node->sock(),m_currentBlock);
				m_currentBlock = QByteArray();

			}

		}
		if (bytesLeft()==0)
			m_current_app_header.MsgType = 0x00;
		return     0;
	}

	//deal to-server handshakes
	bool st_clientNodeAppLayer::Deal_ToServer_Handshakes()
	{
		bool res = true;
		if (m_currentHeader.DataLen < sizeof (PKLTS_App_Layer::tag_app_layer_header))
			return false;
		if (m_currentMessageSize < sizeof(PKLTS_Trans_Header) + sizeof (PKLTS_App_Header))
		{
			// header is not complete, return
			return true;
		}
		//Catch the header
		if (m_current_app_header.MsgType==0x00)
			memcpy((void *)&this->m_current_app_header,
				   ((unsigned char *)this->m_currentBlock.constData()) + sizeof(PKLTS_Trans_Header),
				   sizeof (PKLTS_App_Layer::tag_app_layer_header)
				   );
		switch (m_current_app_header.MsgType)
		{
		case 0x1000:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			qDebug()<<this->peerInfo()<<tr(" Send Regisit:")<<this->m_currentBlock.toHex();
			if (m_currentMessageSize>
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header)
					+ sizeof (stMsg_HostRegistReq)+64)
			{
				emit evt_Message(this,tr("Broken Message, size not correct."));
				res = false;
			}
			else
				res = this->RegisitNewBoxNode();
			break;
		case 0x1001:
			if (bytesLeft()>0)
				// message is not complete, return
				return true;
			qDebug()<<this->peerInfo()<<tr(" Send Login  :")<<this->m_currentBlock.toHex();
			if (m_currentMessageSize>
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header)
					+ sizeof (stMsg_HostLogonReq)+66)
			{
				emit evt_Message(this,tr("Broken Message, size not correct."));
				res = false;
			}
			else
				res = this->LoginHost();
			break;
		default:
			qCritical()<<this->peerInfo()<<tr(" Fatal Handshake :")<<this->m_currentBlock.toHex();
			emit evt_Message(this,tr("Unknown Message:%1").arg(m_current_app_header.MsgType));
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

		if (m_currentHeader.DataLen < sizeof (PKLTS_App_Layer::tag_app_layer_header))
			return false;
		if (m_currentMessageSize < sizeof(PKLTS_Trans_Header) + sizeof (PKLTS_App_Header))
		{
			// header is not complete, return
			return true;
		}
		//Catch the header
		if (m_current_app_header.MsgType==0x00)
			memcpy((void *)&this->m_current_app_header,
				   ((unsigned char *)this->m_currentBlock.constData()) + sizeof(PKLTS_Trans_Header),
				   sizeof (PKLTS_App_Layer::tag_app_layer_header)
				   );
		//do only when all messages has been recieved
		if (bytesLeft()>0)
			return true;
		qDebug()<<this->peerInfo()<<tr(" Send Msg  :")<<this->m_currentBlock.toHex();
		switch (m_current_app_header.MsgType)
		{
		case 0x1002:
			if (m_currentMessageSize!=
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header)
					/*+ sizeof (stMsg_HostTimeCorrectReq)*/)
			{
				qWarning()<<this->peerInfo()<<tr(" Broken Message size not correct  :")<<m_currentMessageSize;
				emit evt_Message(this,tr("Broken Message size not correct."));
				res = false;
			}
			else
				res = this->Box2Svr_CorrectTime();
			break;
		case 0x100B:
			if (m_currentMessageSize<
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header)
					+ sizeof (stMsg_SendDeviceListReq)-1)
			{
				qWarning()<<this->peerInfo()<<tr(" Broken Message size not correct  :")<<m_currentMessageSize;
				emit evt_Message(this,tr("Broken Message size not correct."));
				res = false;
			}
			else
				res = this->RecieveDeviceListFromHost();
			break;
		case 0x100C:
			if (m_currentMessageSize<
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header))
			{
				qWarning()<<this->peerInfo()<<tr(" Broken Message size not correct  :")<<m_currentMessageSize;
				emit evt_Message(this,tr("Broken Message size not correct."));
				res = false;
			}
			else
				res = this->RecieveMacInfoFromHost();
			break;
		case 0x4000:
			if (m_currentMessageSize<
					sizeof(PKLTS_Trans_Header)
					+ sizeof (PKLTS_App_Header))
			{
				qWarning()<<this->peerInfo()<<tr(" Broken Message size not correct  :")<<m_currentMessageSize;
				emit evt_Message(this,tr("Broken Message size not correct."));
				res = false;
			}
			else
				res = this->RecieveEventFromHost();
			break;
		default:
			emit evt_Message(this,tr("Unsupported Message:%1,Bytes:%2").arg(m_current_app_header.MsgType)
							 .arg(QString(m_currentBlock.left(48).toHex())));
			qWarning()<<this->peerInfo()<<tr(" Unsupported:")<<this->m_currentBlock.toHex();
			res = true;
			break;
		}
		m_currentBlock.clear();
		return res;
	}

}
