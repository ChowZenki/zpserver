#include "st_clientnode.h"
#include "st_client_table.h"
namespace SmartLink{
st_clientNode::st_clientNode(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
    zp_plTaskBase(parent)
{
    m_bUUIDRecieved = false;
    m_pClientSock = pClientSock;
    m_uuid [0] = 0;
    m_nUuidHashKey =0;
    m_nSockHashKey = IntegerHash(m_pClientSock);
    m_pCurrentWorker = 0;
    m_btermLater = false;
    m_pClientTable = pClientTable;

}
unsigned int st_clientNode::BKDRHash(const char *str)
{
    register unsigned int hash = 0;
    while (unsigned int ch = (unsigned int)*(str++))
        hash = hash * 131 + ch;
    return hash;
}
unsigned int st_clientNode::IntegerHash(void * ptr)
{
    register unsigned int hash = 0;
    unsigned int sz = sizeof (void *);
    unsigned int val = (unsigned int) ptr;
    for (int i=0;i<sz;i++)
        hash = hash * 131 + (unsigned int)((val>>((sz-1-i)*8)) & 0x0ff);
    return hash;
}

//The main functional method, will run in thread pool
int st_clientNode::run()
{
    if (m_pCurrentWorker!=QThread::currentThread())
    {

        this->moveToThread(QThread::currentThread());
        this->m_pCurrentWorker = QThread::currentThread();
    }
    int nCurrSz = -1;
    int nMessage = m_nMessageBlockSize;
    while (--nMessage>=0 && nCurrSz!=0 )
    {
        QByteArray block;
        m_mutex.lock();
        if (m_list_RawData.size())
        {

            block =  *m_list_RawData.begin();
            m_list_RawData.pop_front();
        }
        nCurrSz = m_list_RawData.size();
        m_mutex.unlock();
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
