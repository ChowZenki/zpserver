#ifndef ZP_NET_THREADPOOL_H
#define ZP_NET_THREADPOOL_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QVector>
#include <QMutex>
#include <QThread>
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>
#include "zp_netlistenthread.h"
#include "zp_nettransthread.h"

namespace ZPNetwork{

class zp_net_ThreadPool : public QObject
{
    Q_OBJECT
public:
    explicit zp_net_ThreadPool(int nPayLoad = 4096,QObject *parent = 0);

    //Listening Ctrl
    //Begin a listening socket at special address and port. The socket will be activated as soon as possible
    void AddListeningAddress(const QString & id,const QHostAddress & address , quint16 nPort,bool bSSLConn = true);
    //Remove a listening socket at special address and port.The socket will be deactivated as soon as possible
    void RemoveListeningAddress(const QString & id);
    //Remove all listening sockets
    void RemoveAllAddresses();


    //Trans Control
    //Add n client-Trans Thread(s).
    void AddClientTransThreads(int nThreads,bool bSSL=true);
    //Remove n client-Trans Thread(s).a thread marked reomved will be terminated after its last client socket exited.
    void RemoveClientTransThreads(int nThreads,bool bSSL=true);
    //Kick All Clients
    void KickAllClients();
    //Deactive Immediately
    void DeactiveImmediately();


    //when Program exit, wait for close;
    bool CanExit();

    //The status
    QStringList ListenerNames();
    int TransThreadNum();
    int totalClients(int idxThread);

protected:
    int m_nPayLoad;
    //QMutex m_mutex_listen;
    //QMutex m_mutex_trans;
    //This map stores listenThreadObjects
    QMap<QString,zp_netListenThread *> m_map_netListenThreads;
    //Internal Threads to hold each listenThreadObjects' message Queue
    QMap<QString,QThread *> m_map_netInternalListenThreads;

    //This map stores ClientTransThreadObjects
    QVector<zp_netTransThread *> m_vec_NetTransThreads;
    //Internal Threads to hold each m_map_NetTransThreads' message Queue
    QVector<QThread *> m_vec_netInternalTransThreads;
    bool TransThreadDel(zp_netTransThread * pThreadObj);
signals:
    //These Message is nessery.-------------------------------------
    void evt_Message(const QString &);
    //The socket error message
    void evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
    //this event indicates new client connected.
    void evt_NewClientConnected(QObject * /*clientHandle*/);
    //this event indicates a client disconnected.
    void evt_ClientDisconnected(QObject * /*clientHandle*/);
    //some data arrival
    void evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
    //a block of data has been successfuly sent
    void evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);

    //Internal Message for ctrl.------------------------------------
    //Listen Control
    void startListen(const QString & id);
    void stopListen(const QString & id);
    void evt_EstablishConnection(QObject * threadid,qintptr socketDescriptor);
    //Trans Control,for intenal thread usage
    void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
    void evt_DeactivteImmediately(zp_netTransThread *);
    void evt_KickAll(zp_netTransThread *);

protected slots:
    void on_New_Arrived_Client(qintptr socketDescriptor);
    void on_ListenClosed(const QString & id);
public slots:
    //Call this function to send data to client
    void SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    //Call this function to send data to client
    void BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);

};
}
#endif // ZP_NET_THREADPOOL_H
