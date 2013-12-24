/*
  zp_netListenThread is a controller class, running in a independance thread.
*/
#ifndef ZP_NETLISTENTHREAD_H
#define ZP_NETLISTENTHREAD_H

#include <QObject>
#include <QHostAddress>
#include "zp_tcpserver.h"
namespace ZPNetwork{
class zp_netListenThread : public QObject
{
    Q_OBJECT
protected:
    ZP_TcpServer * m_tcpServer;
    QString m_id;   //The listen ID
    QHostAddress m_address;
    quint16 m_port;
public:
    explicit zp_netListenThread(const QString & id, QHostAddress address ,quint16 port,QObject *parent = 0);

signals:
    void evt_Message(const QString &);
    void evt_ListenClosed(const QString &);
    //This message will tell thread pool, a new incoming connection has arrived.
    void evt_NewClientArrived(qintptr socketDescriptor);
public slots:
    void startListen(const QString & id);
    void stopListen(const QString & id);
};
}
#endif // ZP_NETLISTENTHREAD_H
