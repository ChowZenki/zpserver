#ifndef ST_CROSS_SVR_NODE_H
#define ST_CROSS_SVR_NODE_H
#include "../cluster/zp_clusternode.h"
namespace SmartLink{

	ZP_Cluster::zp_ClusterNode * st_cross_svr_node_factory(
			ZP_Cluster::zp_ClusterTerm * /*pTerm*/,
			QObject * /*psock*/,
			QObject * /*parent*/);

	void Reg_st_cross_svr_node(ZP_Cluster::zp_ClusterTerm *pTerm );

	class st_cross_svr_node : public ZP_Cluster::zp_ClusterNode
	{
		Q_OBJECT
	public:
		st_cross_svr_node(ZP_Cluster::zp_ClusterTerm * pTerm, QObject * psock,QObject *parent);
	protected:
		//!virtual functions, dealing with the user-defined operations.
		virtual bool deal_user_data(const QByteArray &);
	};
}
#endif // ST_CROSS_SVR_NODE_H
