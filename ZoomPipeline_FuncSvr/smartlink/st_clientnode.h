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

    //deal at most m_nMessageBlockSize messages per deal_message();
    static const int m_nMessageBlockSize = 8;


    //The main functional method, will run in thread pool
    int run();

    //push new binary data into queue
    int push_new_data(const  QByteArray &  dtarray);

    const QString & uuid(){return m_uuid;}
    QObject * sock() {return m_pClientSock;}


    void TerminateLater();

    bool uuidValid(){return m_bUUIDRecieved;}

    //data items
protected:
    //The raw data queue and its mutex
    QList<QByteArray> m_list_RawData;
    QMutex m_mutex;
    //UUID of this equipment
    bool m_bUUIDRecieved;
    QString m_uuid;
    //Client socket handle of this connection
    QObject * m_pClientSock;

    bool m_btermLater;
    st_client_table * m_pClientTable;

signals:
    void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
    void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
};

}

#endif // CLIENTNODE_H
