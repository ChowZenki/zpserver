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

    //Heart beating and healthy
    void KickDealClients();
    int heartBeatingThrd(){return m_nHeartBeatingDeadThrd;}
    void setHeartBeatingThrd(int h) {m_nHeartBeatingDeadThrd = h;}

    //Database and disk resources
    QString Database_UserAcct(){return m_strDBName_useraccount;}
    void setDatabase_UserAcct(const QString & s){m_strDBName_useraccount = s;}
    QString Database_Event(){return m_strDBName_event;}
    void setDatabase_Event(const QString & s){m_strDBName_event = s;}
    QString largeFileFolder(){return m_largeFileFolder;}
    void setLargeFileFolder(const QString & s){m_largeFileFolder = s;}
protected:
    //This list hold dead nodes that still in task queue,avoiding crash
     QList<st_clientNode *> m_nodeToBeDel;

     //Very important hashes. will be improved for cross-server transfer
     QMutex m_hash_mutex;
     QMap<quint32,st_clientNode *> m_hash_uuid2node;
     QMap<QObject *,st_clientNode *> m_hash_sock2node;

     //Concurrent Network frame work
     ZPNetwork::zp_net_ThreadPool * m_pThreadPool;
     //The piple-line
     ZPTaskEngine::zp_pipeline * m_pTaskEngine;

     //The max seconds before dead client be kicked out
     int m_nHeartBeatingDeadThrd;
     //Database Resource Names used by nodes
     QString m_strDBName_useraccount;
     QString m_strDBName_event;
     QString m_largeFileFolder;


signals:
     void evt_Message (const QString &);

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
