#ifndef ZP_CLUSTERTERM_H
#define ZP_CLUSTERTERM_H

#include <QObject>
#include <QHostAddress>
#include <QList>
#include <QMutex>
#include <QMap>
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
#include "../pipeline/zp_pltaskbase.h"

namespace ZP_Cluster{
	class zp_ClusterNode;
	//!this class enable server processes can
	//! communicate with each other.
	class zp_ClusterTerm : public QObject
	{
		Q_OBJECT
	public:
		explicit zp_ClusterTerm(const QString & name,QObject *parent = 0);
		//cluster status
		ZPNetwork::zp_net_ThreadPool * netEng() {return m_pClusterNet;}
		ZPTaskEngine::zp_pipeline * taskEng() {return m_pClusterEng;}
		bool canExit();

		//properties.
		QString setName(const QString & s){ return m_strTermName = s;}
		QString name(){return m_strTermName;}
		QHostAddress publishAddr(){return m_addrPublish;}
		int publishPort(){return m_nPortPublish;}
		QHostAddress setPublishAddr(QHostAddress addr){return m_addrPublish = addr;}
		int setPublishPort(int port){return m_nPortPublish = port;}
		int heartBeatingThrd() {return m_nHeartBeatingDeadThrd;}
		void setHeartBeatingThrd(const int n){m_nHeartBeatingDeadThrd = n;}
	protected:
		QString m_strTermName;//the Terminal's name
		QHostAddress m_addrPublish;	//The publish address for other terms to connect to
		int m_nPortPublish;//The publish port for other terms to connect to
		ZPNetwork::zp_net_ThreadPool * m_pClusterNet;
		ZPTaskEngine::zp_pipeline * m_pClusterEng;
		int m_nHeartBeatingDeadThrd;
		//Server Group Mapping
	protected:
		//This list hold dead nodes that still in task queue,avoiding crash
		QList<zp_ClusterNode *> m_nodeToBeDel;
		//important hashes. server name to socket, socket to server name
		QMutex m_hash_mutex;
		QMap<QString , zp_ClusterNode *> m_hash_Name2node;
		QMap<QObject *,zp_ClusterNode *> m_hash_sock2node;
		bool regisitNewServer(zp_ClusterNode *);
		zp_ClusterNode * SvrNodeFromName(const QString &);
		zp_ClusterNode * SvrNodeFromSocket(QObject *);
	signals:

		void evt_Message(QObject * ,const QString &);
		//The socket error message
		void evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
	protected slots:
		//this event indicates new client connected.
		void on_evt_NewClientConnected(QObject * /*clientHandle*/);
		//this event indicates new client encrypted.
		void on_evt_ClientEncrypted(QObject * /*clientHandle*/);
		//this event indicates a client disconnected.
		void on_evt_ClientDisconnected(QObject * /*clientHandle*/);
		//some data arrival
		void on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
		//a block of data has been successfuly sent
		void on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);

	public slots:
		//!Start listen, this term can be connected by newly joined terms in future.
		void StartListen(const QHostAddress &addr, int nPort);
		//!Join cluster, using existing term (addr:nPort)
		//!as soon as connection established, more existing terms will be sent to this term,
		//!an p2p connection will start
		bool JoinCluster(const QHostAddress &addr, int nPort,bool bSSL=false);
		void  KickDeadClients();

	};
}
#endif // ZP_CLUSTERTERM_H
