#ifndef ZP_NETTRANSTHREAD_H
#define ZP_NETTRANSTHREAD_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QAbstractSocket>
#include <QMutex>
namespace ZPNetwork{
class zp_net_ThreadPool;
class zp_netTransThread : public QObject
{
    Q_OBJECT
public:
    explicit zp_netTransThread(zp_net_ThreadPool * pThreadPool,int nPayLoad = 4096,QObject *parent = 0);

    QList <QObject *> clientsList();
    int CurrentClients();
    void SetPayload(int nPayload);
    bool isActive(){return m_bActivated;}

    bool CanExit();
    bool SSLConnection(){return m_bSSLConnection ;}
    void SetSSLConnection(bool bssl){ m_bSSLConnection = bssl;}

private:
    bool m_bActivated;
    bool m_bSSLConnection;
    //sending buffer, hold byteArraies.
    QMap<QObject *,QList<QByteArray> > m_buffer_sending;

    QMap<QObject *,QList<qint64> > m_buffer_sending_offset;
    QMap<QObject*,int> m_clientList;
    int m_nPayLoad;
    QMutex m_mutex_protect;
    zp_net_ThreadPool * m_pThreadPool;
public slots:
    //This slot dealing with multi-thread client socket accept.
    void incomingConnection(QObject * threadid,qintptr socketDescriptor);
    //sending dtarray to objClient. dtarray will be pushed into m_buffer_sending
    void SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    //Broadcast dtarray to every client except objFromClient itself
    void BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
    //Set terminate mark, the thread will quit after last client quit.
    void Deactivate(){m_bActivated = true;}
    //terminate this thread immediately
    void DeactivateImmediately(zp_netTransThread *);
    //Kick all clients .
    void KickAllClients(zp_netTransThread *);
protected slots:
    //when client closed, this slot will be activated.
    void client_closed();
    void new_data_recieved();
    void some_data_sended(qint64);
    void displayError(QAbstractSocket::SocketError socketError);
    //SSL Encrypted started
    void on_encrypted();
signals:

    void evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
    void evt_NewClientConnected(QObject * client);
    void evt_ClientDisconnected(QObject * client);
    void evt_Data_recieved(QObject * ,const QByteArray &  );
    void evt_Data_transferred(QObject * client,qint64);
};
}
#endif // ZP_NETTRANSTHREAD_H
