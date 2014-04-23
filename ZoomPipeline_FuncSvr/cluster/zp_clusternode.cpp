#include "zp_clusternode.h"
namespace ZP_Cluster{
	zp_ClusterNode::zp_ClusterNode(QObject *parent) :
		ZPTaskEngine::zp_plTaskBase(parent)
	{
	}
	int zp_ClusterNode::run()
	{
		return 0;
	}
}
