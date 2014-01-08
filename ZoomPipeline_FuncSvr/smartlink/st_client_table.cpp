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

 bool st_client_table::regisitClientUUID(st_clientNode * c)
 {
     if (c->uuidValid()==false)
         return false;
     m_hash_mutex.lock();
     m_hash_uuid2node[c->uuid()] = c;
     m_hash_mutex.unlock();
     return true;
 }

 st_clientNode *  st_client_table::clientNodeFromUUID(quint32 uuid)
 {
     m_hash_mutex.lock();
     if (m_hash_uuid2node.contains(uuid))
     {
         m_hash_mutex.unlock();
         return m_hash_uuid2node[uuid];
     }
     m_hash_mutex.unlock();

     return NULL;
 }

 st_clientNode *  st_client_table::clientNodeFromSocket(QObject * sock)
 {
     m_hash_mutex.lock();
     if (m_hash_sock2node.contains(sock))
     {
         m_hash_mutex.unlock();
         return m_hash_sock2node[sock];
     }
     m_hash_mutex.unlock();
     return NULL;

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
    if (nHashContains)
        pClientNode =  m_hash_sock2node[clientHandle];
    if (pClientNode)
    {
        m_hash_sock2node.remove(clientHandle);
        if (pClientNode->uuidValid())
            m_hash_uuid2node.remove(pClientNode->uuid());

        pClientNode->bTermSet = true;
        disconnect (pClientNode,&st_clientNode::evt_SendDataToClient,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::SendDataToClient);
        disconnect (pClientNode,&st_clientNode::evt_BroadcastData,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_BroadcastData);

        m_nodeToBeDel.push_back(pClientNode);
        //qDebug()<<QString("%1(ref %2) Node Push in queue.\n").arg((unsigned int)pClientNode).arg(pClientNode->refCount);
    }
    m_hash_mutex.unlock();

    //Try to delete objects
    QList <st_clientNode *> toBedel;
    foreach(st_clientNode * pdelobj,m_nodeToBeDel)
    {
        if (pdelobj->refCount ==0)
            toBedel.push_back(pdelobj);
        else
        {
           //qDebug()<<QString("%1(ref %2) Waiting in del queue.\n").arg((unsigned int)pdelobj).arg(pdelobj->refCount);
        }
    }
    foreach(st_clientNode * pdelobj,toBedel)
    {
        m_nodeToBeDel.removeAll(pdelobj);

        //qDebug()<<QString("%1(ref %2) deleting.\n").arg((unsigned int)pdelobj).arg(pdelobj->refCount);
        pdelobj->deleteLater();

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
    if (false==nHashContains)
    {
        st_clientNode * pnode = new st_clientNode(this,clientHandle,0);
        //using queued connection of send and revieve;
        connect (pnode,&st_clientNode::evt_SendDataToClient,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::SendDataToClient,Qt::QueuedConnection);
        connect (pnode,&st_clientNode::evt_BroadcastData,m_pThreadPool,&ZPNetwork::zp_net_ThreadPool::evt_BroadcastData,Qt::QueuedConnection);
        m_hash_sock2node[clientHandle] = pnode;
        nHashContains = true;
        pClientNode = pnode;
    }
    else
    {
        pClientNode =  m_hash_sock2node[clientHandle];
    }

    assert(nHashContains!=0 && pClientNode !=0);


    int nblocks =  pClientNode->push_new_data(datablock);
    if (nblocks<=1)
        m_pTaskEngine->pushTask(pClientNode);
    m_hash_mutex.unlock();

}

//a block of data has been successfuly sent
void  st_client_table::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
{

}

}

