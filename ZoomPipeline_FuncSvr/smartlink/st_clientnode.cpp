#include "st_clientnode.h"
#include "st_client_table.h"
namespace SmartLink{
st_clientNode::st_clientNode(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
    zp_plTaskBase(parent)
{
    m_bUUIDRecieved = false;
    m_pClientSock = pClientSock;
    m_uuid = "";
    m_pClientTable = pClientTable;
    bTermSet = false;
}

//The main functional method, will run in thread pool
int st_clientNode::run()
{
    if (bTermSet==true)
    {
        //qDebug()<<QString("%1(%2) Node Martked Deleted, return.\n").arg((unsigned int)this).arg(refCount);
        return 0;
    }
    int nCurrSz = -1;
    int nMessage = m_nMessageBlockSize;
    while (--nMessage>=0 && nCurrSz!=0  )
    {
        QByteArray block;
        m_mutex.lock();
        //Limit max pending blocks.if blocks too long, memory will be low
        while (m_list_RawData.size()>=256)
            m_list_RawData.pop_front();
        if (m_list_RawData.size())
        {

            block =  *m_list_RawData.begin();
            m_list_RawData.pop_front();
        }
        nCurrSz = m_list_RawData.size();
        m_mutex.unlock();
        //Simulate process time cost
        //QThread::currentThread()->msleep(rand()%20+40);
        if (block.isEmpty()==false && block.isNull()==false)
            emit evt_SendDataToClient(this->sock(),block);
    }
    m_mutex.lock();
    nCurrSz = m_list_RawData.size();
    m_mutex.unlock();
    if (nCurrSz==0)
        return 0;
    return -1;
}

//push new binary data into queue
int st_clientNode::push_new_data(const  QByteArray &  dtarray)
{
    int res = 0;
    m_mutex.lock();

    m_list_RawData.push_back(dtarray);
    res = m_list_RawData.size();
    m_mutex.unlock();
    return res;
}

}
