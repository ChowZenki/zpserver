#include "st_cross_svr_node.h"
#include "../cluster/zp_clusterterm.h"
namespace SmartLink{
	ZP_Cluster::zp_ClusterNode * st_cross_svr_node_factory(
				   ZP_Cluster::zp_ClusterTerm * pTerm,
				   QObject * psock,
				   QObject * parent)
	{
		return new st_cross_svr_node(pTerm,psock,parent);
	}

	void Reg_st_cross_svr_node(ZP_Cluster::zp_ClusterTerm *pTerm )
	{
		pTerm->SetNodeFactory(st_cross_svr_node_factory);
	}

	st_cross_svr_node::st_cross_svr_node(ZP_Cluster::zp_ClusterTerm * pTerm, QObject * psock,QObject *parent)
		:ZP_Cluster::zp_ClusterNode(pTerm,psock,parent)
	{
	}
	bool st_cross_svr_node::deal_user_data(const QByteArray &array)
	{
		return ZP_Cluster::zp_ClusterNode::deal_user_data(array);
	}

}
