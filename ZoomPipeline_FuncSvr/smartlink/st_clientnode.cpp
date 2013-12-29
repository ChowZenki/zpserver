#include "st_clientnode.h"
#include "st_client_table.h"
namespace SmartLink{
st_clientNode::st_clientNode(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
    zp_plTaskBase(parent)
{
    m_bUUIDRecieved = false;
    m_pClientSock = pClientSock;
    m_uuid = "";
    m_btermLater = false;
    m_pClientTable = pClientTable;

}
void st_clientNode::TerminateLater()
{
    m_btermLater = true;
}

//The main functional method, will run in thread pool
int st_clientNode::run()
{

    int nCurrSz = -1;
    int nMessage = m_nMessageBlockSize;
    while (--nMessage>=0 && nCurrSz!=0 && m_btermLater==false )
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
