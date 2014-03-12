#include "zp_netlistenthread.h"
namespace ZPNetwork{
	zp_netListenThread::zp_netListenThread(const QString & id, QHostAddress address ,quint16 port,bool bSSL,QObject *parent)
		:QObject(parent)
		,m_tcpServer(0)
		,m_id(id)
		,m_address(address)
		,m_port(port)
		,m_bSSLConn (bSSL)
	{

	}
	void zp_netListenThread::startListen(const QString & id)
	{
		if (id==m_id)
		{
			if (!m_tcpServer)
			{
				m_tcpServer = new ZP_TcpServer(this);
				connect (m_tcpServer,&ZP_TcpServer::evt_NewClientArrived,this,&zp_netListenThread::evt_NewClientArrived,Qt::QueuedConnection);
				if (false==m_tcpServer->listen(m_address,m_port))
				{
					disconnect (m_tcpServer,&ZP_TcpServer::evt_NewClientArrived,this,&zp_netListenThread::evt_NewClientArrived);
					emit evt_Message("Error>"+QString(tr("Can not start listen!")));
					m_tcpServer->deleteLater();
					m_tcpServer = 0;
					//Close this thread.
					emit evt_ListenClosed(m_id);
				}
			}
		}
	}

	void zp_netListenThread::stopListen(const QString & id)
	{
		if (id==m_id)
		{
			if (m_tcpServer)
			{
				disconnect (m_tcpServer,&ZP_TcpServer::evt_NewClientArrived,this,&zp_netListenThread::evt_NewClientArrived);
				m_tcpServer->close();
				m_tcpServer->deleteLater();
				m_tcpServer = 0;
				//Close this thread.
				emit evt_Message("Info>"+QString(tr("Listen Closed!")));
				emit evt_ListenClosed(m_id);
			}
		}
	}
}
