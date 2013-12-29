#include "st_client_table.h"
#include "st_clientnode.h"
#include <assert.h>
#include <functional>
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
void  st_client_table::on_evt_ClientDisconnected(QObject * clientHandle)
{
    int nHashContains = 0;
    st_clientNode * pClientNode = 0;
    m_hash_mutex.lock();
    nHashContains = ZPHashTable::hash_contains(this->m_hash_sock2node,st_clientNode::IntegerHash(clientHandle));
    m_hash_mutex.unlock();
    if (nHashContains)
    {
        m_hash_mutex.lock();
        pClientNode =  (st_clientNode *)ZPHashTable::hash_get(m_hash_sock2node,st_clientNode::IntegerHash(clientHandle),&nHashContains);
        m_hash_mutex.unlock();
    }

    if (pClientNode)
    {
        m_hash_mutex.lock();
        pClientNode->TerminateLater();
        ZPHashTable::hash_del(m_hash_sock2node,st_clientNode::IntegerHash(clientHandle));
        if (pClientNode->uuid()[0])
            ZPHashTable::hash_del(m_hash_uuid2node,pClientNode->uuid_hash());
        m_hash_mutex.unlock();
    }

}

//some data arrival
void  st_client_table::on_evt_Data_recieved(QObject *  clientHandle,const QByteArray & datablock )
{
    //Push Clients to nodes if it is not exist
    int nHashContains = 0;
    st_clientNode * pClientNode = 0;
    m_hash_mutex.lock();
    nHashContains = ZPHashTable::hash_contains(this->m_hash_sock2node,st_clientNode::IntegerHash(clientHandle));
    m_hash_mutex.unlock();
    if (0==nHashContains)
    {
        st_clientNode * pnode = new st_clientNode(this,clientHandle,0);
        connect (pnode,&st_clientNode::evt_SendDataToClient,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::SendDataToClient);
        connect (pnode,&st_clientNode::evt_BroadcastData,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_BroadcastData);
        m_hash_mutex.lock();
        ZPHashTable::hash_set(m_hash_sock2node,st_clientNode::IntegerHash(clientHandle),pnode);
        m_hash_mutex.unlock();
        nHashContains = -1;
        pClientNode = pnode;
    }
    else
    {
        m_hash_mutex.lock();
        pClientNode =  (st_clientNode *)ZPHashTable::hash_get(m_hash_sock2node,st_clientNode::IntegerHash(clientHandle),&nHashContains);
        m_hash_mutex.unlock();
    }

    assert(nHashContains!=0 && pClientNode !=0);


    int nblocks =  pClientNode->push_new_data(datablock);
    if (nblocks==1)
        m_pTaskEngine->pushTask(pClientNode);
}

//a block of data has been successfuly sent
void  st_client_table::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
{

}

}

