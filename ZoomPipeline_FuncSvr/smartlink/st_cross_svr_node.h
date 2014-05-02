#ifndef ST_CROSS_SVR_NODE_H
#define ST_CROSS_SVR_NODE_H
#include "../cluster/zp_clusternode.h"
namespace SmartLink{
	class st_client_table;
	class st_cross_svr_node : public ZP_Cluster::zp_ClusterNode
	{
		Q_OBJECT
	public:
		st_cross_svr_node(ZP_Cluster::zp_ClusterTerm * pTerm, QObject * psock,QObject *parent);
		void setClientTable(st_client_table * table);
	protected:
		//!virtual functions, dealing with the user-defined operations.
		virtual bool deal_user_data(const QByteArray &);
	protected:
		st_client_table * m_pClientTable;
	};
}
#endif // ST_CROSS_SVR_NODE_H
