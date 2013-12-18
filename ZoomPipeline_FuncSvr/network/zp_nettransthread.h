#ifndef ZP_NETTRANSTHREAD_H
#define ZP_NETTRANSTHREAD_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QAbstractSocket>
#include <QMutex>
class zp_netTransThread : public QObject
{
    Q_OBJECT
public:
    explicit zp_netTransThread(int nPayLoad = 4096,QObject *parent = 0);

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
    QMap<QObject *,QList<QByteArray> > m_buffer_sending;
    QMap<QObject *,QList<qint64> > m_buffer_sending_offset;
    QMap<QObject*,int> m_clientList;
    int m_nPayLoad;
    QMutex m_mutex_protect;
public slots:
    //新的客户连接到来
    void incomingConnection(QObject * threadid,qintptr socketDescriptor);
    //向客户端发送数据
    void SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    //向客户端广播数据，不包括 objFromClient
    void BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
    //让本线程不再接受新的连接
    void Deactivate(){m_bActivated = true;}
    //立刻终止本线程的所有连接
    void DeactivateImmediately(zp_netTransThread *);
    //踢出所有客户
    void KickAllClients(zp_netTransThread *);
protected slots:
    //客户连接被关闭
    void client_closed();
    //新的数据到来
    void new_data_recieved();
    //一批数据发送完毕
    void some_data_sended(qint64);
    //客户端错误
    void displayError(QAbstractSocket::SocketError socketError);
    //SSL加密开始
    void on_encrypted();
signals:
    //错误信息
    void evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
    //新的客户端连接
    void evt_NewClientConnected(QObject * client);
    //客户端退出
    void evt_ClientDisconnected(QObject * client);
    //收到一批数据
    void evt_Data_recieved(QObject * ,const QByteArray &  );
    //一批数据被发送
    void evt_Data_transferred(QObject * client,qint64);
};

#endif // ZP_NETTRANSTHREAD_H
