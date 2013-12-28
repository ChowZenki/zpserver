#ifndef CLIENTNODE_H
#define CLIENTNODE_H

#include <QObject>
#include <QList>
#include <QMutex>
namespace SmartLink{

class st_clientNode : public QObject
{
    Q_OBJECT
public:

    explicit st_clientNode( QObject * pClientSock,QObject *parent = 0);

    //The main functional method, will run in thread pool
    int deal_message(int nMessage);

    //push new binary data into queue
    void push_new_data(const  QByteArray &  dtarray);

    unsigned char * uuid(){return m_uuid;}
    QObject * sock() {return m_pClientSock;}

    unsigned int uuid_hash() { return  m_nUuidHashKey;  }
    unsigned int sock_hash() { return  m_nSockHashKey;  }

    //hash value maker
    static unsigned int BKDRHash(const char *str) ;
    static unsigned int IntegerHash(void * ptr);

    //data items
protected:
    //uuid len, uuid is an message header, identifies a special client instument.
    static const int uuid_len = 24;
    //deal at most m_nMessageBlockSize messages per deal_message();
    static const int m_nMessageBlockSize = 8;
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

protected:


signals:
    void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
};

}

#endif // CLIENTNODE_H
