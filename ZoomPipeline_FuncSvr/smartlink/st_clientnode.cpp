#include "st_clientnode.h"
#include "st_client_table.h"
namespace SmartLink{
st_clientNode::st_clientNode(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
    zp_plTaskBase(parent)
{
    m_bUUIDRecieved = false;
    m_currentReadOffset = 0;
    m_currentMessageSize = 0;
    m_pClientSock = pClientSock;
    m_uuid = 0xffffffff;//Not Valid
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
        if (m_list_RawData.size())
            block =  *m_list_RawData.begin();
        m_mutex.unlock();
        if (block.isEmpty()==false && block.isNull()==false)
        {
            m_currentReadOffset = filter_message(block,m_currentReadOffset);
            if (m_currentReadOffset >= block.size())
            {
                m_mutex.lock();
                m_list_RawData.pop_front();
                m_currentReadOffset = 0;
                m_mutex.unlock();
            }
        }
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
//!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
//!return bytes Used.
int st_clientNode::filter_message(const QByteArray & block, int offset)
{
    while (block.length()>offset)
    {
        const char * dataptr = block.constData();
        while (m_currentMessageSize< sizeof(SMARTLINK_MSG))
        {
            m_currentBlock.push_back(dataptr[offset++]);
            m_currentMessageSize++;
            if (offset >= block.length())
                break;
        }
        if (m_currentMessageSize < sizeof(SMARTLINK_MSG)) //Header not completed.
            continue;
        else if (m_currentMessageSize == sizeof(SMARTLINK_MSG))//Header just  completed.
        {
            const char * headerptr = m_currentBlock.constData();
            memcpy((void *)&m_currentHeader,headerptr,sizeof(SMARTLINK_MSG));
            //continue reading if there is data left behind
            if (block.length()>offset)
            {
                qint32 bitLeft = m_currentHeader.payload.data_length + sizeof(SMARTLINK_MSG) - 2
                        -m_currentMessageSize ;
                while (bitLeft>0 && block.length()>offset)
                {
                    m_currentBlock.push_back(dataptr[offset++]);
                    m_currentMessageSize++;
                    bitLeft--;
                }
                //deal block, may be send data as soon as possible;
                deal_current_message_block();
                if (bitLeft>0)
                    continue;
                //This Message is Over. Start a new one.
                m_currentMessageSize = 0;
                m_currentBlock.clear();
                continue;
            }
        }
        else
        {
            if (block.length()>offset)
            {
                qint32 bitLeft = m_currentHeader.payload.data_length + sizeof(SMARTLINK_MSG) - 2
                        -m_currentMessageSize ;
                while (bitLeft>0 && block.length()>offset)
                {
                    m_currentBlock.push_back(dataptr[offset++]);
                    m_currentMessageSize++;
                    bitLeft--;
                }
                //deal block, may be send data as soon as possible;
                deal_current_message_block();
                if (bitLeft>0)
                    continue;
                //This Message is Over. Start a new one.
                m_currentMessageSize = 0;
                m_currentBlock.clear();
                continue;
            }
        }

    }

    return offset;
}

int st_clientNode::deal_current_message_block()
{
    qint32 bitLeft = m_currentHeader.payload.data_length + sizeof(SMARTLINK_MSG) - 2
            -m_currentMessageSize ;
    if (bitLeft<=0)
        emit evt_SendDataToClient(this->sock(),m_currentBlock);
    return 0;
}
}
