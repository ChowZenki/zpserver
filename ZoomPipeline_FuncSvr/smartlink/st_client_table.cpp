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
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_NewClientConnected,this,&st_client_table::on_evt_NewClientConnected,Qt::QueuedConnection);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_ClientDisconnected,this,&st_client_table::on_evt_ClientDisconnected,Qt::QueuedConnection);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_recieved,this,&st_client_table::on_evt_Data_recieved,Qt::QueuedConnection);
    connect (m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_Data_transferred,this,&st_client_table::on_evt_Data_transferred,Qt::QueuedConnection);

}
 st_client_table::~st_client_table()
{
}



//this event indicates new client connected.
void  st_client_table::on_evt_NewClientConnected(QObject * /*clientHandle*/)
{

}

//this event indicates a client disconnected.
void  st_client_table::on_evt_ClientDisconnected(QObject * clientHandle)
{
    bool nHashContains  = false;
    st_clientNode * pClientNode = 0;
    m_hash_mutex.lock();
    nHashContains = m_hash_sock2node.contains(clientHandle);
    m_hash_mutex.unlock();
    if (nHashContains)
    {
        m_hash_mutex.lock();
        pClientNode =  m_hash_sock2node[clientHandle];
        m_hash_mutex.unlock();
    }

    if (pClientNode)
    {
        m_hash_mutex.lock();
        pClientNode->TerminateLater();
        m_hash_sock2node.remove(clientHandle);
        if (pClientNode->uuidValid())
            m_hash_uuid2node.remove(pClientNode->uuid());
        m_pTaskEngine->cancelPendingTask(pClientNode);
        m_hash_mutex.unlock();
    }

}

//some data arrival
void  st_client_table::on_evt_Data_recieved(QObject *  clientHandle,const QByteArray & datablock )
{
    //Push Clients to nodes if it is not exist
    bool nHashContains = 0;
    st_clientNode * pClientNode = 0;
    m_hash_mutex.lock();
    nHashContains = m_hash_sock2node.contains(clientHandle);
    m_hash_mutex.unlock();
    if (false==nHashContains)
    {
        st_clientNode * pnode = new st_clientNode(this,clientHandle,0);
        //using queued connection of send and revieve;
        connect (pnode,&st_clientNode::evt_SendDataToClient,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::SendDataToClient,Qt::QueuedConnection);
        connect (pnode,&st_clientNode::evt_BroadcastData,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_BroadcastData,Qt::QueuedConnection);
        m_hash_mutex.lock();
        m_hash_sock2node[clientHandle] = pnode;
        m_hash_mutex.unlock();
        nHashContains = true;
        pClientNode = pnode;
    }
    else
    {
        m_hash_mutex.lock();
        pClientNode =  m_hash_sock2node[clientHandle];
        m_hash_mutex.unlock();
    }

    assert(nHashContains!=0 && pClientNode !=0);


    int nblocks =  pClientNode->push_new_data(datablock);
    if (nblocks<=1)
        m_pTaskEngine->pushTask(pClientNode);
}

//a block of data has been successfuly sent
void  st_client_table::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
{

}

}

