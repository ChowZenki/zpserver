#include "st_client_table.h"
namespace SmartLink{
st_client_table::st_client_table(ZPNetwork::zp_net_ThreadPool * pool, ZPTaskEngine::zp_pipeline * taskeng,QObject *parent) :
    QObject(parent)
  ,m_pThreadPool(pool)
  ,m_pTaskEngine(taskeng)
{
    m_hash_id2sock =  ZPHashTable::hash_init(2,1);
    m_hash_sock2id =  ZPHashTable::hash_init(2,1);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_NewClientConnected,this,&st_client_table::on_evt_NewClientConnected);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_ClientDisconnected,this,&st_client_table::on_evt_ClientDisconnected);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_recieved,this,&st_client_table::on_evt_Data_recieved);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_transferred,this,&st_client_table::on_evt_Data_transferred);

}
 st_client_table::~st_client_table()
{
    ZPHashTable::hash_fini(m_hash_id2sock);
    ZPHashTable::hash_fini(m_hash_sock2id);
    m_hash_sock2id = 0;
    m_hash_id2sock = 0;
}

unsigned int st_client_table::BKDRHash(const char *str)
{
    register unsigned int hash = 0;
    while (unsigned int ch = (unsigned int)*(str++))
        hash = hash * 131 + ch;
    return hash;
}

//this event indicates new client connected.
void  st_client_table::on_evt_NewClientConnected(QObject * /*clientHandle*/)
{

}

//this event indicates a client disconnected.
void  st_client_table::on_evt_ClientDisconnected(QObject * /*clientHandle*/)
{

}

//some data arrival
void  st_client_table::on_evt_Data_recieved(QObject *  clientHandle,const QByteArray & datablock )
{
    this->m_pThreadPool->SendDataToClient(clientHandle,datablock);
    //push some tasks
    m_pTaskEngine->pushTask([](void)->int {
        //QThread::currentThread()->msleep(20);
        return 0;
    });
}

//a block of data has been successfuly sent
void  st_client_table::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
{

}

}

