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

    //!deal at most m_nMessageBlockSize messages per deal_message();
    static const int m_nMessageBlockSize = 8;
    //!The main functional method, will run in thread pool
    int run();

    //push new binary data into queue
    int push_new_data(const  QByteArray &  dtarray);

    quint32 uuid(){return m_uuid;}
    QObject * sock() {return m_pClientSock;}
    bool uuidValid(){return m_bUUIDRecieved;}
    bool bTermSet;
protected:
    //!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
    //!return bytes Used.
    int filter_message(const QByteArray &, int offset);
    int deal_current_message_block();
    //data items
protected:
    //The current Read Offset, from m_list_RawData's beginning
    int m_currentReadOffset;
    //Current Message Offset, according to m_currentHeader
    int m_currentMessageSize;
    //Current un-procssed message block.for large blocks,
    //this array will be re-setted as soon as some part of data has been
    //dealed, eg, send a 200MB block, the 200MB data will be splitted into pieces
    QByteArray m_currentBlock;
    //current Header
    SMARTLINK_MSG m_currentHeader;
    //The raw data queue and its mutex
    QList<QByteArray> m_list_RawData;
    QMutex m_mutex;

    //UUID of this equipment
    bool m_bUUIDRecieved;
    quint64 m_uuid;  //Client ID
    //Client socket handle of this connection
    QObject * m_pClientSock;

    st_client_table * m_pClientTable;

signals:
    void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
};

}

#endif // CLIENTNODE_H
