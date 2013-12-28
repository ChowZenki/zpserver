#include "st_client_table.h"
namespace SmartLink{
st_client_table::st_client_table(ZPNetwork::zp_net_ThreadPool * pool, ZPTaskEngine::zp_pipeline * taskeng,QObject *parent) :
    QObject(parent)
  ,m_pThreadPool(pool)
  ,m_pTaskEngine(taskeng)
{
    m_hash_uuid2node =  ZPHashTable::hash_init(2,1);
    m_hash_sock2node =  ZPHashTable::hash_init(2,1);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_NewClientConnected,this,&st_client_table::on_evt_NewClientConnected);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_ClientDisconnected,this,&st_client_table::on_evt_ClientDisconnected);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_recieved,this,&st_client_table::on_evt_Data_recieved);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_transferred,this,&st_client_table::on_evt_Data_transferred);

}
 st_client_table::~st_client_table()
{
    ZPHashTable::hash_fini(m_hash_uuid2node);
    ZPHashTable::hash_fini(m_hash_sock2node);
    m_hash_sock2node = 0;
    m_hash_uuid2node = 0;
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

