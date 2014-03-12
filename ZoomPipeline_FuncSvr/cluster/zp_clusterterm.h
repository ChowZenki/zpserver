#ifndef ZP_CLUSTERTERM_H
#define ZP_CLUSTERTERM_H

#include <QObject>
#include <QHostAddress>
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
#include "../pipeline/zp_pltaskbase.h"

namespace ZP_Cluster{
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
	protected:
		QString m_strTermName;//the Terminal's name
		ZPNetwork::zp_net_ThreadPool * m_pClusterNet;
		ZPTaskEngine::zp_pipeline * m_pClusterEng;
	signals:

	public slots:
		//!Start listen, this term can be connected by newly joined terms in future.
		void StartListen(const QHostAddress &addr, int nPort);
		//!Join cluster, using existing term (addr:nPort)
		//!as soon as connection established, more existing terms will be sent to this term,
		//!an p2p connection will start
		bool JoinCluster(const QHostAddress &addr, int nPort);

	};
}
#endif // ZP_CLUSTERTERM_H
