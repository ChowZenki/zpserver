#ifndef ST_CLIENT_TABLE_H
#define ST_CLIENT_TABLE_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMap>
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
#include "./st_message.h"
namespace SmartLink{
class st_clientNode;
class st_client_table : public QObject
{
    Q_OBJECT
public:
    explicit st_client_table( ZPNetwork::zp_net_ThreadPool * pool, ZPTaskEngine::zp_pipeline * taskeng,QObject *parent = 0);
    ~st_client_table();

    bool regisitClientUUID(st_clientNode *);
    st_clientNode * clientNodeFromUUID(quint32);
    st_clientNode * clientNodeFromSocket(QObject *);

protected:
     QList<st_clientNode *> m_nodeToBeDel;
     QMutex m_hash_mutex;
     QMap<quint32,st_clientNode *> m_hash_uuid2node;
     QMap<QObject *,st_clientNode *> m_hash_sock2node;
     ZPNetwork::zp_net_ThreadPool * m_pThreadPool;
     ZPTaskEngine::zp_pipeline * m_pTaskEngine;
signals:

public slots:
     //this event indicates new client connected.
     void on_evt_NewClientConnected(QObject * /*clientHandle*/);
     //this event indicates a client disconnected.
     void on_evt_ClientDisconnected(QObject * /*clientHandle*/);
     //some data arrival
     void on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
     //a block of data has been successfuly sent
     void on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);

};
}
#endif // ST_CLIENT_TABLE_H
