#include "zp_clusterterm.h"
namespace ZP_Cluster{
	zp_ClusterTerm::zp_ClusterTerm(const QString & name,QObject *parent ) :
		QObject(parent)
	  ,m_strTermName(name)
	{
		m_pClusterEng = new ZPTaskEngine::zp_pipeline(this);
		m_pClusterNet = new ZPNetwork::zp_net_ThreadPool(8192,this);
		connect(m_pClusterNet,&ZPNetwork::zp_net_ThreadPool::evt_Message, this,&zp_ClusterTerm::evt_Message);
		connect(m_pClusterNet,&ZPNetwork::zp_net_ThreadPool::evt_SocketError, this,&zp_ClusterTerm::evt_SocketError);



		m_nPortPublish = 0;

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
	//this event indicates new client connected.
	void  zp_ClusterTerm::on_evt_NewClientConnected(QObject * /*clientHandle*/)
	{

	}

	//this event indicates new client encrypted.
	void  zp_ClusterTerm::on_evt_ClientEncrypted(QObject * /*clientHandle*/)
	{

	}

	//this event indicates a client disconnected.
	void  zp_ClusterTerm::on_evt_ClientDisconnected(QObject * /*clientHandle*/)
	{

	}

	//some data arrival
	void  zp_ClusterTerm::on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ )
	{

	}

	//a block of data has been successfuly sent
	void  zp_ClusterTerm::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
	{

	}
}
