#ifndef ST_CLIENT_TABLE_H
#define ST_CLIENT_TABLE_H

#include <QObject>
#include "../dynhash/dynhash.h"
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
namespace SmartLink{
class st_client_table : public QObject
{
    Q_OBJECT
public:
    explicit st_client_table( ZPNetwork::zp_net_ThreadPool * pool, ZPTaskEngine::zp_pipeline * taskeng,QObject *parent = 0);
    ~st_client_table();
    static unsigned int BKDRHash(const char *str) ;
protected:
     ZPHashTable::CDYNHASHTB m_hash_id2sock;
     ZPHashTable::CDYNHASHTB m_hash_sock2id;
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
