#include "st_clientnode.h"
#include "st_client_table.h"

namespace SmartLink{
bool st_clientNode::Deal_BoxToServer_Messages()
{
    //The bytes left to recieve.
    qint32 bytesLeft = m_currentHeader.data_length + sizeof(SMARTLINK_MSG) - 1
            -m_currentMessageSize ;

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
        if (bytesLeft>0)
            // message is not complete, return
            return true;
        if (m_currentMessageSize!=
                sizeof(SMARTLINK_MSG) - 1
                + sizeof (SMARTLINK_MSG_APP::tag_app_layer_header)
                + sizeof (stMsg_HostRegistReq))
        {
            emit evt_Message(tr("Broken Message stMsg_HostRegistReq, size not correct."));
            emit evt_close_client(this->sock());
            return false;
        }
        return this->RegisitNewNode();
        break;
    default:
        emit evt_Message(tr("Message type not supported."));
        emit evt_close_client(this->sock());
        break;
    }

    m_currentBlock.clear();


    return true;
}
}
