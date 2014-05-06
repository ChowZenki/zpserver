#ifndef CLIENTNODE_H
#define CLIENTNODE_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include "st_clientnode_basetrans.h"
#include "st_msg_applayer.h"
namespace ExampleServer{
	class st_client_table;
	class st_clientNodeAppLayer : public st_clientNode_baseTrans
	{
		Q_OBJECT
		friend class st_clien_table;
	public:
		explicit st_clientNodeAppLayer(st_client_table * pClientTable, QObject * pClientSock,QObject *parent = 0);
	protected:
		bool loadRelations();
		bool saveRelations();

		//!in App-Layer, it Split the messages into dealers.
		int deal_current_message_block();
		//!Message Dealers, imp in st_clientnode_msgdeal.cpp
		bool Deal_ToServer_Handshakes();
		//0x3000<->0x3800 msg, stMsg_ClientLoginReq
		bool LoginClient();
		//!Deal node2Svr Msgs
		bool Deal_Node2Svr_Msgs();
		//0x1003 - 0x1803
		bool Box2Svr_UploadUserTable();
		//0x1004 - 0x1804
		bool Box2Svr_DownloadUserTable();
		//0x3001 - 0x3801
		bool ClientLogout();
		//data items
	protected:
		QMutex m_mutex_equipID;
		EXAMPLE_MSG_APP m_current_app_header;
		//log in
		bool m_bLoggedIn;

		//Matching Nodes
		QSet<quint32> m_matched_nodes;


	};

}

#endif // CLIENTNODE_H
