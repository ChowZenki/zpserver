#ifndef CLIENTNODE_H
#define CLIENTNODE_H

#include <QObject>
#include <QList>
#include <QMutex>
#include "st_message.h"
#include "../pipeline/zp_pltaskbase.h"
namespace SmartLink{
class st_client_table;
class st_clientNode : public ZPTaskEngine::zp_plTaskBase
{
    Q_OBJECT
    friend class st_clien_table;
public:

    explicit st_clientNode(st_client_table * pClientTable, QObject * pClientSock,QObject *parent = 0);

    //uuid len, uuid is an message header, identifies a special client instument.
    static const int uuid_len = SMARTLINK_UUID_LEN;
    //deal at most m_nMessageBlockSize messages per deal_message();
    static const int m_nMessageBlockSize = 8;


    //The main functional method, will run in thread pool
    int run();

    //push new binary data into queue
    int push_new_data(const  QByteArray &  dtarray);

    unsigned char * uuid(){return m_uuid;}
    QObject * sock() {return m_pClientSock;}

    unsigned int uuid_hash() { return  m_nUuidHashKey;  }
    unsigned int sock_hash() { return  m_nSockHashKey;  }

    //hash value maker
    static unsigned int BKDRHash(const char *str) ;
    static unsigned int IntegerHash(void * ptr);

    QObject * currentWorker() {return m_pCurrentWorker;}

    void TerminateLater(){m_btermLater = true;}

    //data items
protected:
    //The raw data queue and its mutex
    QList<QByteArray> m_list_RawData;
    QMutex m_mutex;
    //UUID of this equipment
    bool m_bUUIDRecieved;
    unsigned char m_uuid[uuid_len];
    unsigned int m_nUuidHashKey;
    //Client socket handle of this connection
    QObject * m_pClientSock;
    unsigned int m_nSockHashKey;

    //current worker
     QObject *  m_pCurrentWorker;
    bool m_btermLater;
    st_client_table * m_pClientTable;

signals:
    void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
};

}

#endif // CLIENTNODE_H
