#include "st_clientnode.h"
namespace SmartLink{
st_clientNode::st_clientNode( QObject * pClientSock ,QObject *parent) :
    QObject(parent)
{
    m_bUUIDRecieved = false;
    m_pClientSock = pClientSock;
    m_uuid [0] = 0;
    m_nUuidHashKey =0;
    m_nSockHashKey = IntegerHash(m_pClientSock);

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
        hash = hash * 131 + (unsigned int)((val>>(i*8)) & 0x0ff);
    return hash;
}

//The main functional method, will run in thread pool
int st_clientNode::deal_message(int nMessage)
{
    return -1;
}

//push new binary data into queue
void st_clientNode::push_new_data(const  QByteArray &  dtarray)
{
    m_mutex.lock();
    m_list_RawData.push_back(dtarray);
    m_mutex.unlock();
}

}
