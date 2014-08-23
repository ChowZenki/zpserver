#ifndef CLIENTNODE_H
#define CLIENTNODE_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include "st_clientnode_basetrans.h"
#include "st_message.h"
namespace ParkinglotsSvr{
	class st_client_table;
	class st_clientNodeAppLayer : public st_clientNode_baseTrans
	{
		Q_OBJECT
		friend class st_clien_table;
	public:
		explicit st_clientNodeAppLayer(st_client_table * pClientTable, QObject * pClientSock,QObject *parent = 0);
	protected:
		//!in App-Layer, it Split the messages into dealers.
		int deal_current_message_block();
		//!Message Dealers, imp in st_clientnode_msgdeal.cpp
		bool Deal_ToServer_Handshakes();
		bool RegisitNewBoxNode();
		//0x0001<->0x7FFE msg, stMsg_ClientLoginReq
		bool LoginHost();
		//!Deal node2Svr Msgs
		bool Deal_Node2Svr_Msgs();
		bool Box2Svr_CorrectTime();
		bool RecieveDeviceListFromHost();
		bool RecieveMacInfoFromHost();
		//data items
	protected:
		QMutex m_mutex_equipID;
		PKLTS_APP_HEADER m_current_app_header;
		//log in
		bool m_bLoggedIn;
		QString m_serialNum;
		stMsg_SendMacInfoReq_internal m_macInfo;
	};

}

#endif // CLIENTNODE_H
