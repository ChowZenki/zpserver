#include "zp_net_threadpool.h"

zp_net_ThreadPool::zp_net_ThreadPool(int nPayLoad,QObject *parent) :
    QObject(parent)
{
    m_nPayLoad = nPayLoad;
    //The signals and slots will be reged.
    if (false==QMetaType::isRegistered(QMetaType::type("qintptr")))
        qRegisterMetaType<qintptr>("qintptr");
    if (false==QMetaType::isRegistered(QMetaType::type("QAbstractSocket::SocketError")))
        qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}
QStringList zp_net_ThreadPool::ListenerNames()
{
    return m_map_netListenThreads.keys();
}

int zp_net_ThreadPool::TransThreadNum()
{
    return m_vec_NetTransThreads.size();
}

int zp_net_ThreadPool::totalClients(int idxThread)
{
    int nsz = m_vec_NetTransThreads.size();
    if (idxThread >=0 && idxThread<nsz)
        return m_vec_NetTransThreads[idxThread]->CurrentClients();
    else
        return 0;
}

//Begin a listening socket at special address and port. The socket will be activated as soon as possible
void zp_net_ThreadPool::AddListeningAddress(const QString & id,const QHostAddress & address , quint16 nPort)
{
    if (m_map_netListenThreads.find(id)==m_map_netListenThreads.end())
    {
        //Start Thread
        QThread * pThread = new QThread(this);
        zp_netListenThread * pListenObj = new zp_netListenThread(id,address,nPort);
        pThread->start();
        //m_mutex_listen.lock();
        m_map_netInternalListenThreads[id] = pThread;
        m_map_netListenThreads[id] = pListenObj;
        //m_mutex_listen.unlock();
        //Bind Object to New thread
        connect(this,&zp_net_ThreadPool::startListen,pListenObj,&zp_netListenThread::startListen);
        connect(this,&zp_net_ThreadPool::stopListen,pListenObj,&zp_netListenThread::stopListen);
        connect(pListenObj,&zp_netListenThread::evt_Message,this,&zp_net_ThreadPool::evt_Message);
        connect(pListenObj,&zp_netListenThread::evt_ListenClosed,this,&zp_net_ThreadPool::on_ListenClosed);
        connect(pListenObj,&zp_netListenThread::evt_NewClientArrived,this,&zp_net_ThreadPool::on_New_Arrived_Client);

        pListenObj->moveToThread(pThread);
        //Start Listen Immediately
        emit startListen(id);
    }
    else
        emit evt_Message("Warning>"+QString(tr("This ID has been used.")));
}

//Remove a listening socket at special address and port.The socket will be deactivated as soon as possible
void zp_net_ThreadPool::RemoveListeningAddress(const QString & id)
{
    //m_mutex_listen.lock();
    if (m_map_netListenThreads.find(id)!=m_map_netListenThreads.end())
        emit stopListen(id);
    //m_mutex_listen.unlock();
}
void zp_net_ThreadPool::RemoveAllAddresses()
{
    //m_mutex_listen.lock();
    foreach (QString id,m_map_netListenThreads.keys())
        emit stopListen(id);
    //m_mutex_listen.unlock();
}

void zp_net_ThreadPool::on_New_Arrived_Client(qintptr socketDescriptor)
{
    emit evt_Message("Info>"+QString(tr("New Client Arriverd.")));
    //m_mutex_trans.lock();
    int nsz = m_vec_NetTransThreads.size();
    int nMinPay = 0x7fffffff;
    int nMinIdx = -1;
    for (int i=0;i<nsz && nMinPay!=0;i++)
    {
        if (m_vec_NetTransThreads[i]->isActive()==false)
            continue;
        int nPat = m_vec_NetTransThreads[i]->CurrentClients();

        if (nPat<nMinPay)
        {
            nMinPay = nPat;
            nMinIdx = i;
        }
        //qDebug()<<i<<" "<<nPat<<" "<<nMinIdx;
    }
    if (nMinIdx>=0 && nMinIdx<nsz)
        emit evt_EstablishConnection(m_vec_NetTransThreads[nMinIdx],socketDescriptor);
    //m_mutex_trans.unlock();
}
void zp_net_ThreadPool::on_ListenClosed(const QString & id)
{
    //m_mutex_listen.lock();
    if (m_map_netListenThreads.find(id)!=m_map_netListenThreads.end())
    {
        //Clean objects;
        zp_netListenThread * pListenObj = m_map_netListenThreads[id];
        QThread * pThread = m_map_netInternalListenThreads[id];
        m_map_netListenThreads.remove(id);
        m_map_netInternalListenThreads.remove(id);
        //disconnect signals;
        disconnect(this,&zp_net_ThreadPool::startListen,pListenObj,&zp_netListenThread::startListen);
        disconnect(this,&zp_net_ThreadPool::stopListen,pListenObj,&zp_netListenThread::stopListen);
        disconnect(pListenObj,&zp_netListenThread::evt_Message,this,&zp_net_ThreadPool::evt_Message);
        disconnect(pListenObj,&zp_netListenThread::evt_ListenClosed,this,&zp_net_ThreadPool::on_ListenClosed);
        disconnect(pListenObj,&zp_netListenThread::evt_NewClientArrived,this,&zp_net_ThreadPool::on_New_Arrived_Client);
        pListenObj->deleteLater();
        pThread->quit();
        pThread->deleteLater();

    }
    //m_mutex_listen.unlock();
}
//Add n client-Trans Thread(s).
void zp_net_ThreadPool::AddClientTransThreads(int nThreads)
{
    if (nThreads>0 && nThreads<256)
    {
        for (int i=0;i<nThreads;i++)
        {
            zp_netTransThread * clientTH = new zp_netTransThread(m_nPayLoad);
            QThread * pThread = new QThread(this);
            //m_mutex_trans.lock();
            m_vec_netInternalTransThreads.push_back(pThread);
            m_vec_NetTransThreads.push_back(clientTH);
            //m_mutex_trans.unlock();
            pThread->start();
            //Connect signals
            connect (clientTH,&zp_netTransThread::evt_ClientDisconnected,this,&zp_net_ThreadPool::evt_ClientDisconnected);
            connect (clientTH,&zp_netTransThread::evt_Data_recieved,this,&zp_net_ThreadPool::evt_Data_recieved);
            connect (clientTH,&zp_netTransThread::evt_Data_transferred,this,&zp_net_ThreadPool::evt_Data_transferred);
            connect (clientTH,&zp_netTransThread::evt_NewClientConnected,this,&zp_net_ThreadPool::evt_NewClientConnected);
            connect (clientTH,&zp_netTransThread::evt_SocketError,this,&zp_net_ThreadPool::evt_SocketError);
            connect (this,&zp_net_ThreadPool::evt_EstablishConnection,clientTH,&zp_netTransThread::incomingConnection);
            connect (this,&zp_net_ThreadPool::evt_BroadcastData,clientTH,&zp_netTransThread::BroadcastData);
            connect (this,&zp_net_ThreadPool::evt_SendDataToClient,clientTH,&zp_netTransThread::SendDataToClient);
            connect (this,&zp_net_ThreadPool::evt_KickAll,clientTH,&zp_netTransThread::KickAllClients);
            connect (this,&zp_net_ThreadPool::evt_DeactivteImmediately,clientTH,&zp_netTransThread::DeactivateImmediately);

            clientTH->moveToThread(pThread);
        }
    }
}
bool zp_net_ThreadPool::TransThreadDel(zp_netTransThread * pThreadObj)
{
    if (pThreadObj->CanExit()==false)
        return false;
    int nsz = m_vec_NetTransThreads.size();
    int idx = -1;
    for (int i=0;i<nsz && idx<0;i++)
    {
        if (m_vec_NetTransThreads[i]==pThreadObj)
            idx = i;
    }
    if (idx>=0 && idx <nsz)
    {
        zp_netTransThread * clientTH =  m_vec_NetTransThreads[idx];
        disconnect (clientTH,&zp_netTransThread::evt_ClientDisconnected,this,&zp_net_ThreadPool::evt_ClientDisconnected);
        disconnect (clientTH,&zp_netTransThread::evt_Data_recieved,this,&zp_net_ThreadPool::evt_Data_recieved);
        disconnect (clientTH,&zp_netTransThread::evt_Data_transferred,this,&zp_net_ThreadPool::evt_Data_transferred);
        disconnect (clientTH,&zp_netTransThread::evt_NewClientConnected,this,&zp_net_ThreadPool::evt_NewClientConnected);
        disconnect (clientTH,&zp_netTransThread::evt_SocketError,this,&zp_net_ThreadPool::evt_SocketError);
        disconnect (this,&zp_net_ThreadPool::evt_EstablishConnection,clientTH,&zp_netTransThread::incomingConnection);
        disconnect (this,&zp_net_ThreadPool::evt_BroadcastData,clientTH,&zp_netTransThread::BroadcastData);
        disconnect (this,&zp_net_ThreadPool::evt_SendDataToClient,clientTH,&zp_netTransThread::SendDataToClient);
        disconnect (this,&zp_net_ThreadPool::evt_KickAll,clientTH,&zp_netTransThread::KickAllClients);
        disconnect (this,&zp_net_ThreadPool::evt_DeactivteImmediately,clientTH,&zp_netTransThread::DeactivateImmediately);
        m_vec_netInternalTransThreads[idx]->quit();
        m_vec_netInternalTransThreads[idx]->deleteLater();
        m_vec_NetTransThreads[idx]->deleteLater();
        m_vec_netInternalTransThreads.remove(idx);
        m_vec_NetTransThreads.remove(idx);

    }
    return true;
}
void zp_net_ThreadPool::KickAllClients()
{
    //m_mutex_trans.lock();
    int nsz = m_vec_NetTransThreads.size();
    for (int i=0;i<nsz;i++)
        emit evt_KickAll(m_vec_NetTransThreads[i]);
    //m_mutex_trans.unlock();
}
void zp_net_ThreadPool::DeactiveImmediately()
{
    //m_mutex_trans.lock();
    int nsz = m_vec_NetTransThreads.size();
    for (int i=0;i<nsz;i++)
        emit evt_DeactivteImmediately(m_vec_NetTransThreads[i]);
    //m_mutex_trans.unlock();
}

//Remove n client-Trans Thread(s).a thread marked reomved will be terminated after its last client socket exited.
void zp_net_ThreadPool::RemoveClientTransThreads(int nThreads)
{
    if (nThreads>0)
    {
        //m_mutex_trans.lock();
        int nsz = m_vec_NetTransThreads.size();
        for (int i=0;i<nsz && i<nThreads;i++)
            m_vec_NetTransThreads[i]->Deactivate();
        //m_mutex_trans.unlock();
    }

}

void zp_net_ThreadPool::SendDataToClient(QObject * objClient,const QByteArray &  dtarray)
{
    emit evt_SendDataToClient(objClient,dtarray);
}

//向客户端广播数据，不包括 objFromClient
void zp_net_ThreadPool::BroadcastData(QObject * objFromClient,const QByteArray &  dtarray)
{
    emit evt_BroadcastData(objFromClient,dtarray);
}


bool zp_net_ThreadPool::CanExit()
{
    bool res = true;
    //m_mutex_trans.lock();
    int nsz = m_vec_NetTransThreads.size();
    for (int i=nsz-1;i>=0 && res==true;i--)
        res = TransThreadDel( m_vec_NetTransThreads[i]);
    //m_mutex_trans.unlock();

    //m_mutex_listen.lock();
    if (m_map_netListenThreads.size())
        res= false;
    //m_mutex_listen.unlock();
    return res;
}
