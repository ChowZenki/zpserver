#include "st_clientnode.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <assert.h>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSettings>
#include <QSqlError>
namespace SmartLink{
//0x0001 msg, stMsg_HostRegistReq
bool st_clientNode::RegisitNewNode()
{
    SMARTLINK_MSG_APP * pAppLayer =
            (SMARTLINK_MSG_APP *)(
                ((const char *)(m_currentBlock.constData()))
                +sizeof(SMARTLINK_MSG)-1);
    //form Msgs
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_HostRegistRsp);
    QByteArray array(sizeof(SMARTLINK_MSG) + nMsgLen - 1,0);
    char * ptr = array.data();
    SMARTLINK_MSG * pMsg = (SMARTLINK_MSG *)ptr;
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );
    pMsg->Mark = 0x55AA;
    pMsg->version = m_currentHeader.version;
    pMsg->SerialNum = m_currentHeader.SerialNum;
    pMsg->Priority = m_currentHeader.Priority;
    pMsg->Reserved1 = 0;
    pMsg->source_id = (quint32)((quint64)(m_currentHeader.destin_id) & 0xffffffff );

    pMsg->destin_id = (quint32)((quint64)(m_currentHeader.source_id) & 0xffffffff );;

    pMsg->data_length = nMsgLen;
    pMsg->Reserved2 = 0;


    pApp->header.AskID = m_current_app_header.header.AskID;
    pApp->header.MsgType = 0x1800;
    pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

    stMsg_HostRegistRsp & reply = pApp->MsgUnion.msg_HostRegistRsp;

    //Check the database, find current equipment info
    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

    reply.DoneCode = 2;
    reply.ID = 0xffffffff;
    strcpy(reply.TextInfo,"Unknown error");
    if (db.isValid()==true && db.isOpen()==true )
    {
        QSqlQuery query(db);
        QString strSerial ;
        for (int i=0;i<64;i++)
        {
            strSerial+= pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i];
        }
        QString sql = "select host_serial_num,equip_id from instruments where host_serial_num = ?;";
        query.prepare(sql);
        query.addBindValue(strSerial);

        if (true==query.exec())
        {
            if (query.next())
            {
                bool bOk = false;
                int ncurrid = query.value(1).toInt(&bOk);
                if (bOk==true)
                {
                    if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
                    {
                        reply.ID = ncurrid;
                        reply.DoneCode = 1;
                        strcpy(reply.TextInfo,"Re-regisit Succeed.");
                    }
                    else
                    {
                        reply.ID = AssignNewEquipID(strSerial);
                        if (reply.ID>=0x0010000 && reply.ID <=0x0FFFFFFF)
                        {
                            reply.DoneCode = 0;
                            strcpy(reply.TextInfo,"First-regisit Succeed.");
                        }
                        else
                            strcpy(reply.TextInfo,"Equip ID resource error.");
                    }
                }
                else
                    strcpy(reply.TextInfo,"Raw Dev ID Is Invalid.");
            }
            else
            {
                // No such device
                strcpy(reply.TextInfo,"No such device ID.");
            }
        }
        else
        {
            strcpy(reply.TextInfo,"Server Access Error.");
            emit evt_Message(tr("Database Access Error :")+query.lastError().text());
        }
    }
    else
    {
        //Server db is currently not accessable, wait.
        strcpy(reply.TextInfo,"Server Not Accessable Now.");
    }


    //Send back
    emit evt_SendDataToClient(this->sock(),array);



    return reply.DoneCode==2?false:true;
}

quint32 st_clientNode::AssignNewEquipID(const QString & serial)
{
    QString config_file = QCoreApplication::applicationDirPath();
    config_file += "/serial.ini";
    QSettings settings(config_file,QSettings::IniFormat);
    QMutexLocker locker(&m_mutex_equipID);
    quint32 id = settings.value("counter/serial",(quint32)0x10000).toUInt();
    settings.setValue("counter/serial",(quint32)(id+1));

    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
    QSqlQuery query(db);
    QString strSql = QString ("update instruments set equip_id = ? where host_serial_num = ?;");
    query.prepare(strSql);
    query.addBindValue(id);
    query.addBindValue(serial);
    if (false==query.exec())
    {
        id = 0;
        emit evt_Message(tr("Database Access Error :")+query.lastError().text());

    }
    return id;
}
bool st_clientNode::LoginSvr()
{
    SMARTLINK_MSG_APP * pAppLayer =
            (SMARTLINK_MSG_APP *)(
                ((const char *)(m_currentBlock.constData()))
                +sizeof(SMARTLINK_MSG)-1);
    //form Msgs
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_HostLogonRsp);
    QByteArray array(sizeof(SMARTLINK_MSG) + nMsgLen - 1,0);
    char * ptr = array.data();
    SMARTLINK_MSG * pMsg = (SMARTLINK_MSG *)ptr;
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );
    pMsg->Mark = 0x55AA;
    pMsg->version = m_currentHeader.version;
    pMsg->SerialNum = m_currentHeader.SerialNum;
    pMsg->Priority = m_currentHeader.Priority;
    pMsg->Reserved1 = 0;
    pMsg->source_id = (quint32)((quint64)(m_currentHeader.destin_id) & 0xffffffff );

    pMsg->destin_id = (quint32)((quint64)(m_currentHeader.source_id) & 0xffffffff );;

    pMsg->data_length = nMsgLen;
    pMsg->Reserved2 = 0;


    pApp->header.AskID = m_current_app_header.header.AskID;
    pApp->header.MsgType = 0x1801;
    pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

    stMsg_HostLogonRsp & reply = pApp->MsgUnion.msg_HostLogonRsp;

    //Check the database, find current equipment info
    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

    reply.DoneCode = 3;
    strcpy(reply.TextInfo,"Unknown error");
    if (db.isValid()==true && db.isOpen()==true )
    {
        QSqlQuery query(db);
        QString strSerial ;
        for (int i=0;i<64;i++)
        {
            strSerial+= pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[i];
        }
        QString sql = "select host_serial_num,equip_id from instruments where host_serial_num = ?;";
        query.prepare(sql);
        query.addBindValue(strSerial);

        if (true==query.exec())
        {
            if (query.next())
            {
                bool bOk = false;
                int ncurrid = query.value(1).toInt(&bOk);
                if (bOk==true)
                {
                    if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
                    {
                        if (pAppLayer->MsgUnion.msg_HostLogonReq.ID==ncurrid)
                        {
                            reply.TextInfo[0] = 0;
                            reply.DoneCode = 0;
                        }
                        else
                            strcpy(reply.TextInfo,"ID Not matched.");
                    }
                    else
                        strcpy(reply.TextInfo,"Equip ID has not been regisited.");
                }
                else
                    strcpy(reply.TextInfo,"Raw Dev ID Is Invalid.");
            }
            else
            {
                // No such device
                strcpy(reply.TextInfo,"No such device ID.");
            }
        }
        else
        {
            strcpy(reply.TextInfo,"Server Access Error.");
            emit evt_Message(tr("Database Access Error :")+query.lastError().text());
        }
    }
    else
    {
        //Server db is currently not accessable, wait.
        strcpy(reply.TextInfo,"Server Not Accessable Now.");
    }


    //Send back
    emit evt_SendDataToClient(this->sock(),array);



    return reply.DoneCode==0?true:false;
}
}
