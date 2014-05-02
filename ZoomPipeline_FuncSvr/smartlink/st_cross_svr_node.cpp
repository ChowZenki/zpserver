#include "st_cross_svr_node.h"
#include "../cluster/zp_clusterterm.h"
#include "st_client_table.h"
namespace SmartLink{

	st_cross_svr_node::st_cross_svr_node(ZP_Cluster::zp_ClusterTerm * pTerm, QObject * psock,QObject *parent)
		:ZP_Cluster::zp_ClusterNode(pTerm,psock,parent)
	{
	}
	bool st_cross_svr_node::deal_user_data(const QByteArray &array)
	{
		return ZP_Cluster::zp_ClusterNode::deal_user_data(array);
	}
	void st_cross_svr_node::setClientTable(st_client_table * table)
	{
		this->m_pClientTable = table;
	}

}
