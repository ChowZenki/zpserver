#include "zp_clusterterm.h"
#include "zp_clusternode.h"
#include <assert.h>
namespace ZP_Cluster{
	zp_ClusterTerm::zp_ClusterTerm(const QString & name,QObject *parent ) :
		QObject(parent)
	  ,m_strTermName(name)
	{
		m_pClusterEng = new ZPTaskEngine::zp_pipeline(this);
		m_pClusterNet = new ZPNetwork::zp_net_Engine(8192,this);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_Message, this,&zp_ClusterTerm::evt_Message,Qt::QueuedConnection);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_SocketError, this,&zp_ClusterTerm::evt_SocketError,Qt::QueuedConnection);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_Data_recieved, this,&zp_ClusterTerm::on_evt_Data_recieved,Qt::QueuedConnection);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_Data_transferred, this,&zp_ClusterTerm::on_evt_Data_transferred,Qt::QueuedConnection);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_ClientDisconnected, this,&zp_ClusterTerm::on_evt_ClientDisconnected,Qt::QueuedConnection);
		connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_NewClientConnected, this,&zp_ClusterTerm::on_evt_NewClientConnected,Qt::QueuedConnection);
		//connect(m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_ClientEncrypted, this,&zp_ClusterTerm::on_evt_ClientEncrypted);
		m_nPortPublish = 0;
		m_nHeartBeatingTime = 20;

	}
	int zp_ClusterTerm::publishPort(){

		return m_nPortPublish;
	}
	ZPNetwork::zp_net_Engine * zp_ClusterTerm::netEng()
	{
		return m_pClusterNet;
	}
	ZPTaskEngine::zp_pipeline * zp_ClusterTerm::taskEng()
	{
		return m_pClusterEng;
	}
	QString zp_ClusterTerm::setName(const QString & s)
	{
		return m_strTermName = s;
	}
	QString zp_ClusterTerm::name()
	{
		return m_strTermName;
	}
	QHostAddress zp_ClusterTerm::publishAddr()
	{
		return m_addrPublish;
	}
	QHostAddress zp_ClusterTerm::setPublishAddr(QHostAddress addr)
	{
		return m_addrPublish = addr;
	}
	int zp_ClusterTerm::setPublishPort(int port)
	{
		return m_nPortPublish = port;
	}
	int zp_ClusterTerm::heartBeatingThrdHold()
	{
		return m_nHeartBeatingTime;
	}
	void zp_ClusterTerm::setHeartBeatingThrd(const int n)
	{
		m_nHeartBeatingTime = n;
	}
	void zp_ClusterTerm::StartListen(const QHostAddress &addr, int nPort)
	{
		m_pClusterNet->AddListeningAddress(m_strTermName,addr,nPort,false);

	}
	bool zp_ClusterTerm::JoinCluster(const QHostAddress &addr, int nPort,bool bSSL)
	{
		return m_pClusterNet->connectTo(addr,nPort,bSSL);
	}
	bool zp_ClusterTerm::canExit()
	{
		return m_pClusterEng->canClose() && m_pClusterNet->CanExit();
	}
	QStringList zp_ClusterTerm::SvrNames()
	{
		QStringList lst;
		m_hash_mutex.lock();
		QList<QString> keys =  m_hash_Name2node.keys();
		//Msgs
		foreach (QString key,keys)
		{
			lst.push_back(m_hash_Name2node[key]->termName());
		}
		m_hash_mutex.unlock();
		return lst;
	}
	QHostAddress zp_ClusterTerm::SvrAddr(const QString & name)
	{
		QHostAddress addr;
		m_hash_mutex.lock();
		if (m_hash_Name2node.contains(name))
			addr = m_hash_Name2node[name]->addrPublish();
		m_hash_mutex.unlock();
		return addr;
	}

	int zp_ClusterTerm::SvrPort(const QString & name)
	{
		int port = 0;
		m_hash_mutex.lock();
		if (m_hash_Name2node.contains(name))
			port = m_hash_Name2node[name]->portPublish();
		m_hash_mutex.unlock();
		return port;
	}

	bool zp_ClusterTerm::regisitNewServer(zp_ClusterNode * c)
	{
		//Before reg, termname must be recieved.
		if (c->termName().length()<1)
		{
			emit evt_Message(c,tr("Name is empty!")+c->termName());
			return false;
		}
		m_hash_mutex.lock();
		if (m_hash_Name2node.contains(c->termName())==true)
		{
			m_hash_mutex.unlock();
			return false;
		}
		m_hash_Name2node[c->termName()] = c;
		m_hash_mutex.unlock();
		return true;
	}

	zp_ClusterNode * zp_ClusterTerm::SvrNodeFromName(const QString & uuid)
	{
		m_hash_mutex.lock();
		if (m_hash_Name2node.contains(uuid))
		{
			m_hash_mutex.unlock();
			return m_hash_Name2node[uuid];
		}
		m_hash_mutex.unlock();

		return NULL;
	}

	zp_ClusterNode * zp_ClusterTerm::SvrNodeFromSocket(QObject * sock)
	{
		m_hash_mutex.lock();
		if (m_hash_sock2node.contains(sock))
		{
			m_hash_mutex.unlock();
			return m_hash_sock2node[sock];
		}
		m_hash_mutex.unlock();
		return NULL;
	}

	//this event indicates new client connected.
	void  zp_ClusterTerm::on_evt_NewClientConnected(QObject * clientHandle)
	{
		bool nHashContains = false;
		zp_ClusterNode * pClientNode = 0;
		m_hash_mutex.lock();
		nHashContains = m_hash_sock2node.contains(clientHandle);
		if (false==nHashContains)
		{
			zp_ClusterNode * pnode = new zp_ClusterNode(this,clientHandle,0);
			//using queued connection of send and revieve;
			connect (pnode,&zp_ClusterNode::evt_SendDataToClient,m_pClusterNet,&ZPNetwork::zp_net_Engine::SendDataToClient,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_BroadcastData,m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_BroadcastData,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_close_client,m_pClusterNet,&ZPNetwork::zp_net_Engine::KickClients,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_Message,this,&zp_ClusterTerm::evt_Message,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_connect_to,m_pClusterNet,&ZPNetwork::zp_net_Engine::connectTo,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_NewSvrConnected,this,&zp_ClusterTerm::evt_NewSvrConnected,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_RemoteData_recieved,this,&zp_ClusterTerm::evt_RemoteData_recieved,Qt::QueuedConnection);
			m_hash_sock2node[clientHandle] = pnode;
			nHashContains = true;
			pClientNode = pnode;
		}
		else
		{
			pClientNode =  m_hash_sock2node[clientHandle];
		}
		m_hash_mutex.unlock();
		assert(nHashContains!=0 && pClientNode !=0);
		//Send Hello Package
		pClientNode->SendHelloPackage();
	}

	//this event indicates new client encrypted.
	void  zp_ClusterTerm::on_evt_ClientEncrypted(QObject * clientHandle)
	{
		bool nHashContains = false;
		zp_ClusterNode * pClientNode = 0;
		m_hash_mutex.lock();
		nHashContains = m_hash_sock2node.contains(clientHandle);
		if (false==nHashContains)
		{
			zp_ClusterNode * pnode = new zp_ClusterNode(this,clientHandle,0);
			//using queued connection of send and revieve;
			connect (pnode,&zp_ClusterNode::evt_SendDataToClient,m_pClusterNet,&ZPNetwork::zp_net_Engine::SendDataToClient,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_BroadcastData,m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_BroadcastData,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_close_client,m_pClusterNet,&ZPNetwork::zp_net_Engine::KickClients,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_Message,this,&zp_ClusterTerm::evt_Message,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_connect_to,m_pClusterNet,&ZPNetwork::zp_net_Engine::connectTo,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_NewSvrConnected,this,&zp_ClusterTerm::evt_NewSvrConnected,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_RemoteData_recieved,this,&zp_ClusterTerm::evt_RemoteData_recieved,Qt::QueuedConnection);
			m_hash_sock2node[clientHandle] = pnode;
			nHashContains = true;
			pClientNode = pnode;
		}
		else
		{
			pClientNode =  m_hash_sock2node[clientHandle];
		}
		m_hash_mutex.unlock();
		assert(nHashContains!=0 && pClientNode !=0);
		//Send Hello Package
		pClientNode->SendHelloPackage();
	}

	//this event indicates a client disconnected.
	void  zp_ClusterTerm::on_evt_ClientDisconnected(QObject * clientHandle)
	{
		bool nHashContains  = false;

		zp_ClusterNode * pClientNode = 0;
		m_hash_mutex.lock();
		nHashContains = m_hash_sock2node.contains(clientHandle);
		if (nHashContains)
			pClientNode =  m_hash_sock2node[clientHandle];
		if (pClientNode)
		{
			QString nameCurr = pClientNode->termName();
			disconnect (pClientNode,&zp_ClusterNode::evt_SendDataToClient,m_pClusterNet,&ZPNetwork::zp_net_Engine::SendDataToClient);
			disconnect (pClientNode,&zp_ClusterNode::evt_BroadcastData,m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_BroadcastData);
			disconnect (pClientNode,&zp_ClusterNode::evt_close_client,m_pClusterNet,&ZPNetwork::zp_net_Engine::KickClients);
			disconnect (pClientNode,&zp_ClusterNode::evt_Message,this,&zp_ClusterTerm::evt_Message);
			disconnect (pClientNode,&zp_ClusterNode::evt_connect_to,m_pClusterNet,&ZPNetwork::zp_net_Engine::connectTo);
			disconnect (pClientNode,&zp_ClusterNode::evt_NewSvrConnected,this,&zp_ClusterTerm::evt_NewSvrConnected);
			disconnect (pClientNode,&zp_ClusterNode::evt_RemoteData_recieved,this,&zp_ClusterTerm::evt_RemoteData_recieved);
			m_hash_sock2node.remove(clientHandle);
			if (pClientNode->termName().length()>0)
			{
				//This is important. some time m_hash_Name2node and m_hash_sock2node, same uuid has different socket.
				if (m_hash_Name2node.contains(pClientNode->termName()))
					if (m_hash_Name2node[pClientNode->termName()]==pClientNode)
						m_hash_Name2node.remove(pClientNode->termName());
			}

			pClientNode->bTermSet = true;
			m_nodeToBeDel.push_back(pClientNode);
			if (nameCurr.length()>0)
				emit evt_NewSvrDisconnected(nameCurr);
			//qDebug()<<QString("%1(ref %2) Node Push in queue.\n").arg((unsigned int)pClientNode).arg(pClientNode->ref());
		}
		m_hash_mutex.unlock();

		//Try to delete objects
		QList <zp_ClusterNode *> toBedel;
		foreach(zp_ClusterNode * pdelobj,m_nodeToBeDel)
		{
			if (pdelobj->ref() ==0)
				toBedel.push_back(pdelobj);
			else
			{
				//qDebug()<<QString("%1(ref %2) Waiting in del queue.\n").arg((unsigned int)pdelobj).arg(pdelobj->ref());
			}
		}
		foreach(zp_ClusterNode * pdelobj,toBedel)
		{
			m_nodeToBeDel.removeAll(pdelobj);
			//qDebug()<<QString("%1(ref %2) deleting.\n").arg((unsigned int)pdelobj).arg(pdelobj->ref());
			pdelobj->deleteLater();
		}
	}

	//some data arrival
	void  zp_ClusterTerm::on_evt_Data_recieved(QObject *  clientHandle,const QByteArray & datablock )
	{
		//Push Clients to nodes if it is not exist
		bool nHashContains = false;
		zp_ClusterNode * pClientNode = 0;
		m_hash_mutex.lock();
		nHashContains = m_hash_sock2node.contains(clientHandle);
		if (false==nHashContains)
		{
			zp_ClusterNode * pnode = new zp_ClusterNode(this,clientHandle,0);
			//using queued connection of send and revieve;
			connect (pnode,&zp_ClusterNode::evt_SendDataToClient,m_pClusterNet,&ZPNetwork::zp_net_Engine::SendDataToClient,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_BroadcastData,m_pClusterNet,&ZPNetwork::zp_net_Engine::evt_BroadcastData,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_close_client,m_pClusterNet,&ZPNetwork::zp_net_Engine::KickClients,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_Message,this,&zp_ClusterTerm::evt_Message,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_connect_to,m_pClusterNet,&ZPNetwork::zp_net_Engine::connectTo,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_NewSvrConnected,this,&zp_ClusterTerm::evt_NewSvrConnected,Qt::QueuedConnection);
			connect (pnode,&zp_ClusterNode::evt_RemoteData_recieved,this,&zp_ClusterTerm::evt_RemoteData_recieved,Qt::QueuedConnection);
			m_hash_sock2node[clientHandle] = pnode;
			nHashContains = true;
			pClientNode = pnode;
		}
		else
		{
			pClientNode =  m_hash_sock2node[clientHandle];
		}
		assert(nHashContains!=0 && pClientNode !=0);
		int nblocks =  pClientNode->push_new_data(datablock);
		if (nblocks<=1)
			m_pClusterEng->pushTask(pClientNode);
		m_hash_mutex.unlock();
	}
	void  zp_ClusterTerm::KickDeadClients()
	{
		m_hash_mutex.lock();
		for (QMap<QObject *,zp_ClusterNode *>::iterator p =m_hash_sock2node.begin();
			 p!=m_hash_sock2node.end();p++)
		{
			p.value()->CheckHeartBeating();
		}
		m_hash_mutex.unlock();
	}
	//a block of data has been successfuly sent
	void  zp_ClusterTerm::on_evt_Data_transferred(QObject *   clientHandle, qint64 bytesent)
	{
		emit evt_RemoteData_transferred(clientHandle,bytesent);
	}
	void zp_ClusterTerm::BroadcastServers()
	{
		if (name().length()<1)
			return;
		m_hash_mutex.lock();
		QList<QString> keys =  m_hash_Name2node.keys();
		int nsz = keys.size();
		//Msgs
		int nMsgLen = sizeof(CROSS_SVR_MSG::tag_header) + sizeof (CROSS_SVR_MSG::uni_payload::tag_CSM_Broadcast) * (nsz+1);
		QByteArray array(nMsgLen,0);
		CROSS_SVR_MSG * pMsg =(CROSS_SVR_MSG *) array.data();
		pMsg->hearder.Mark = 0x1234;
		pMsg->hearder.data_length = sizeof (CROSS_SVR_MSG::uni_payload::tag_CSM_Broadcast) * (nsz+1);
		pMsg->hearder.messagetype = 0x02;
		int ct = 0;
		foreach (QString key,keys)
		{
			zp_ClusterNode * pNode = m_hash_Name2node[key];
			strncpy((char *)pMsg->payload.broadcastMsg[ct].name,
					pNode->termName().toStdString().c_str(),
					sizeof(pMsg->payload.broadcastMsg[ct].name)-1);
			strncpy((char *)pMsg->payload.broadcastMsg[ct].Address,
					pNode->addrPublish().toString().toStdString().c_str(),
					sizeof(pMsg->payload.broadcastMsg[ct].Address)-1);
			pMsg->payload.broadcastMsg[ct].port = pNode->portPublish();
			++ct;
		}
		m_hash_mutex.unlock();
		strncpy((char *)pMsg->payload.broadcastMsg[ct].name,
				this->name().toStdString().c_str(),
				sizeof(pMsg->payload.broadcastMsg[ct].name)-1);
		strncpy((char *)pMsg->payload.broadcastMsg[ct].Address,
				this->publishAddr().toString().toStdString().c_str(),
				sizeof(pMsg->payload.broadcastMsg[ct].Address)-1);
		pMsg->payload.broadcastMsg[ct].port = this->publishPort();
		m_hash_mutex.lock();
		QList<QString> sockkeys =  m_hash_Name2node.keys();
		//Msgs
		foreach (QString key,sockkeys)
		{
			netEng()->SendDataToClient(m_hash_Name2node[key]->sock(),array);
		}
		m_hash_mutex.unlock();
	}

	void zp_ClusterTerm::SendHeartBeatings()
	{
		int nMsgLen = sizeof(CROSS_SVR_MSG::tag_header);
		QByteArray array(nMsgLen,0);
		CROSS_SVR_MSG * pMsg =(CROSS_SVR_MSG *) array.data();
		pMsg->hearder.Mark = 0x1234;
		pMsg->hearder.data_length = 0;
		pMsg->hearder.messagetype = 0x00;
		//m_pClusterNet->BroadcastData(0,array);

		m_hash_mutex.lock();
		QList<QString> keys =  m_hash_Name2node.keys();
		//Msgs
		foreach (QString key,keys)
		{
			netEng()->SendDataToClient(m_hash_Name2node[key]->sock(),array);
		}
		m_hash_mutex.unlock();
	}
	void zp_ClusterTerm::SendDataToRemoteServer(const QString & svrName,const QByteArray & SourceArray)
	{
		int nMsgLen = sizeof(CROSS_SVR_MSG::tag_header) +  SourceArray.size();
		QByteArray array(nMsgLen,0);
		CROSS_SVR_MSG * pMsg =(CROSS_SVR_MSG *) array.data();
		pMsg->hearder.Mark = 0x1234;
		pMsg->hearder.data_length = SourceArray.size();
		pMsg->hearder.messagetype = 0x03;
		memcpy (pMsg->payload.data,SourceArray.constData(),SourceArray.size());
		m_hash_mutex.lock();
		if (m_hash_Name2node.contains(svrName))
			netEng()->SendDataToClient(m_hash_Name2node[svrName]->sock(),array);
		m_hash_mutex.unlock();
	}
}
