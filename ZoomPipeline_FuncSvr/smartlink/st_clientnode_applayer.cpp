#include "st_clientnode_applayer.h"
#include "st_client_table.h"
namespace SmartLink{
st_clientNodeAppLayer::st_clientNodeAppLayer(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
    st_clientNode_baseTrans(pClientTable,pClientSock,parent)
{

    m_bLoggedIn= false;
    memset(&m_current_app_header,0,sizeof(SMARTLINK_MSG_APP));
}


//!deal current message
int st_clientNodeAppLayer::deal_current_message_block()
{
    //then , Start deal to-server messages
    //Server - deal messages
    if (m_currentHeader.destin_id==0x00000001)
    {
        if (this->m_bLoggedIn==false || this->m_bUUIDRecieved==false)
        {
            //To-Server Messages does not wait for message-block completes
            if (false==Deal_ToServer_Handshakes())
            {
                m_currentBlock = QByteArray();
                emit evt_Message(tr("To-server Message Failed."));
                emit evt_close_client(this->sock());
             }
        }
        else
        {
            m_currentBlock = QByteArray();
            emit evt_Message(tr("We will imp these functions in future."));
        }

    }
    //deal Broadcast messages
    else if (m_currentHeader.destin_id==0xFFFFFFFC)
    {
        //need furture works.
        //Do Nothing
         emit evt_Message(tr("Broadcast Message is not currently supported."));
        m_currentBlock = QByteArray();
    }
    else if (m_currentHeader.destin_id==0xFFFFFFFD)
    {
        //need furture works.
        //Do Nothing
        emit evt_Message(tr("Broadcast Message is not currently supported."));
        m_currentBlock = QByteArray();
    }
    else
    {
        //find Destin Client using Hash.
        st_clientNode_baseTrans * destin_node = m_pClientTable->clientNodeFromUUID(m_currentHeader.destin_id);
        if (destin_node==NULL)
        {
            //need further dev, insert into db, or catched on disk.
            //destin client is un-reachable, or in another function server.
            //need server-to-server channels to re-post this message.
            emit evt_Message(tr("Destin ID ") + QString("%1").arg(m_currentHeader.destin_id) + tr(" is not currently logged in."));

            //Do Nothing
        }
        else
        {
            emit evt_SendDataToClient(destin_node->sock(),m_currentBlock);
            m_currentBlock = QByteArray();

        }

    }
    if (bytesLeft()==0)
         m_current_app_header.header.MsgType = 0x00;
    return     st_clientNode_baseTrans::deal_current_message_block();
}

//deal to-server handshakes
bool st_clientNodeAppLayer::Deal_ToServer_Handshakes()
{
    bool res = true;

    if (m_currentHeader.data_length < sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
        return false;
    if (m_currentMessageSize < sizeof(SMARTLINK_MSG) - 1 + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header))
    {
        // header is not complete, return
        return true;
    }
    //Catch the header
    if (m_current_app_header.header.MsgType==0x00)
        memcpy((void *)&this->m_current_app_header,
               ((unsigned char *)this->m_currentBlock.constData()) + sizeof(SMARTLINK_MSG) - 1,
               sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
               );
    if (m_current_app_header.header.MsgFmtVersion!=0x01)
    {
        emit evt_Message(tr("Application Layer Version too new."));
        emit evt_close_client(this->sock());
        return false;
    }
    //do
    switch (m_current_app_header.header.MsgType)
    {
    case 0x1000:
        if (bytesLeft()>0)
            // message is not complete, return
            return true;
        if (m_currentMessageSize!=
                sizeof(SMARTLINK_MSG) - 1
                + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
                + sizeof (stMsg_HostRegistReq))
        {
            emit evt_Message(tr("Broken Message stMsg_HostRegistReq, size not correct."));
            res = false;
        }
        else
            res = this->RegisitNewBoxNode();
        break;
    case 0x1001:
        if (bytesLeft()>0)
            // message is not complete, return
            return true;
        if (m_currentMessageSize!=
                sizeof(SMARTLINK_MSG) - 1
                + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
                + sizeof (stMsg_HostLogonReq))
        {
            emit evt_Message(tr("Broken Message stMsg_HostLogonReq, size not correct."));
            res = false;
        }
        else
            res = this->LoginBox();
        break;
    case 0x3000:
        if (bytesLeft()>0)
            // message is not complete, return
            return true;
        if (m_currentMessageSize!=
                sizeof(SMARTLINK_MSG) - 1
                + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
                + sizeof (stMsg_ClientLoginReq))
        {
            emit evt_Message(tr("Broken Message stMsg_ClientLoginReq, size not correct."));
            res = false;
        }
        else
            res = this->LoginClient();
        break;
    default:
        emit evt_Message(tr("Message type not supported."));
        res = false;
        break;
    }

    m_currentBlock.clear();


    return res;
}
}
