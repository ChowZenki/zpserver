#ifndef ZP_CLUSTERNODE_H
#define ZP_CLUSTERNODE_H

#include <QObject>
#include "../pipeline/zp_pltaskbase.h"
namespace ZP_Cluster{
	/**
	 * @brief This class stand for a remote server.
	 * when local server establish a connection between itself and remote svr,
	 * a zp_ClusterNode object will be created.
	 */
	class zp_ClusterNode : public ZPTaskEngine::zp_plTaskBase
	{
		Q_OBJECT
	public:
		explicit zp_ClusterNode(QObject *parent = 0);
		int run();
	signals:

	public slots:

	};
}
#endif // ZP_CLUSTERNODE_H
