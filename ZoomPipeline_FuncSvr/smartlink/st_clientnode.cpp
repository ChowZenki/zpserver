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
        else
        {
            m_mutex.lock();
            //pop empty cabs
            if (m_list_RawData.empty()==false)
                m_list_RawData.pop_front();
            m_mutex.unlock();
        }
        m_mutex.lock();
        nCurrSz = m_list_RawData.size();
        m_mutex.unlock();
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
        while (m_currentMessageSize< sizeof(SMARTLINK_MSG)-1)
        {
            m_currentBlock.push_back(dataptr[offset++]);
            m_currentMessageSize++;
            if (offset >= block.length())
                break;
        }
        if (m_currentMessageSize < sizeof(SMARTLINK_MSG)-1) //Header not completed.
            continue;
        else if (m_currentMessageSize == sizeof(SMARTLINK_MSG)-1)//Header just  completed.
        {
            const char * headerptr = m_currentBlock.constData();
            memcpy((void *)&m_currentHeader,headerptr,sizeof(SMARTLINK_MSG)-1);

            //continue reading if there is data left behind
            if (block.length()>offset)
            {
                qint32 bitLeft = m_currentHeader.data_length + sizeof(SMARTLINK_MSG) - 1
                        -m_currentMessageSize ;
                while (bitLeft>0 && block.length()>offset)
                {
                    m_currentBlock.push_back(dataptr[offset++]);
                    m_currentMessageSize++;
                    bitLeft--;
                }
                if (m_currentHeader.Mark==0x55AA )
                    //deal block, may be send data as soon as possible;
                    deal_current_message_block();
                else //Bad MSG!
                {
                    emit evt_close_client(this->sock());
                }
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
                qint32 bitLeft = m_currentHeader.data_length + sizeof(SMARTLINK_MSG) - 1
                        -m_currentMessageSize ;
                while (bitLeft>0 && block.length()>offset)
                {
                    m_currentBlock.push_back(dataptr[offset++]);
                    m_currentMessageSize++;
                    bitLeft--;
                }
                //deal block, may be processed as soon as possible;
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
//!deal current message
int st_clientNode::deal_current_message_block()
{

    //First, get uuid as soon as possible
    if (m_bUUIDRecieved==false)
    {
        if (m_currentHeader.source_id!=0xffffffff)
        {
            m_bUUIDRecieved = true;
            m_uuid =  m_currentHeader.source_id;
            //regisit client node to hash-table;
            m_pClientTable->regisitClientUUID(this);
        }
        else //Invalid
            return 0;
    }

    //then , Start deal to-server messages
    if (m_currentHeader.destin_id==0xffffffff)
    {
        //need furture works.
        if (m_currentHeader.data_length==0) //heart-beating
        {
            emit evt_SendDataToClient(this->sock(),m_currentBlock);
            m_currentBlock.clear();
        }
        else
        {
            //Do Nothing
            m_currentBlock.clear();
        }
    }
    //deal client-to-client messages
    else
    {
        //find Destin Client using Hash.
        st_clientNode * destin_node = m_pClientTable->clientNodeFromUUID(m_currentHeader.destin_id);
        if (destin_node==NULL)
        {
            //need further dev, insert into db, or catched on disk.
            //destin client is un-reachable, or in another function server.
            //need server-to-server channels to re-post this message.
            qDebug()<<"Destin ID "<<m_currentHeader.destin_id<< "is not valid\n";
            //Do Nothing
        }
        else
        {
            emit evt_SendDataToClient(destin_node->sock(),m_currentBlock);
            m_currentBlock.clear();
        }

    }
    return 0;
}
}
