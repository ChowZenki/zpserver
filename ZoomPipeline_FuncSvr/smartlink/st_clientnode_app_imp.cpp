#include "st_clientnode_applayer.h"
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
bool st_clientNodeAppLayer::RegisitNewBoxNode()
{
    const SMARTLINK_MSG_APP * pAppLayer =
            (const SMARTLINK_MSG_APP *)(
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
        for (int i=0;i<64 && pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i]!=0 ;i++)
        {
            strSerial+= pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i];
            m_serialNum[i] =  pAppLayer->MsgUnion.msg_HostRegistReq.HostSerialNum[i];
        }
        QString sql = "select host_serial_num,equip_id,first_login from instruments where host_serial_num = ?;";
        query.prepare(sql);
        query.addBindValue(strSerial);

        if (true==query.exec())
        {
            if (query.next())
            {
                bool bOk = false;
                int ncurrid = query.value(1).toInt(&bOk);
                int nfirstlogin =  query.value(2).toInt();
                if (bOk==true)
                {
                    if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
                    {
                        reply.ID = ncurrid;
                        reply.DoneCode = nfirstlogin==1?0:1;
                        strcpy(reply.TextInfo,"Re-regisit Succeed.");
                        m_bUUIDRecieved = true;
                        m_uuid = ncurrid;
                        m_pClientTable->regisitClientUUID(this);
                        if (nfirstlogin==1)
                        {
                            strcpy(reply.TextInfo,"First-Regisit Succeed.");
                            QSqlQuery queryUpdate(db);
                            sql = "update instruments set first_login = 0 where  host_serial_num = ?;";
                            queryUpdate.prepare(sql);
                            queryUpdate.addBindValue(strSerial);
                            if (false==queryUpdate.exec())
                            {
                                 reply.DoneCode = 2;
                                 strcpy(reply.TextInfo,"Internal Server Error!");
                            }
                        }

                    }
                    else
                    {
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


bool st_clientNodeAppLayer::LoginBox()
{
    const SMARTLINK_MSG_APP * pAppLayer =
            (const SMARTLINK_MSG_APP *)(
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
        for (int i=0;i<64 && pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[i]!=0;i++)
        {
            strSerial+= pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[i];
            m_serialNum[i] = pAppLayer->MsgUnion.msg_HostLogonReq.HostSerialNum[i];
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
                            m_bLoggedIn = true;
                            m_bUUIDRecieved = true;
                            m_uuid = ncurrid;
                            m_pClientTable->regisitClientUUID(this);

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

bool st_clientNodeAppLayer::LoginClient()
{
    const SMARTLINK_MSG_APP * pAppLayer =
            (const SMARTLINK_MSG_APP *)(
                ((const char *)(m_currentBlock.constData()))
                +sizeof(SMARTLINK_MSG)-1);
    //form Msgs
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_ClientLoginRsp);
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
    pApp->header.MsgType = 0x3800;
    pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

    stMsg_ClientLoginRsp & reply = pApp->MsgUnion.msg_ClientLoginRsp;

    //Check the database, find current equipment info
    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

    reply.DoneCode = 3;
    strcpy(reply.TextInfo,"Unknown error");
    if (db.isValid()==true && db.isOpen()==true )
    {
        QSqlQuery query(db);
        QString strUserName, strPasswd ;
        for (int i=0;i<32 && pAppLayer->MsgUnion.msg_ClientLoginReq.UserName[i]!=0;i++)
        {
            m_username[i] = pAppLayer->MsgUnion.msg_ClientLoginReq.UserName[i];
            strUserName+= pAppLayer->MsgUnion.msg_ClientLoginReq.UserName[i];
        }
        for (int i=0;i<32 && pAppLayer->MsgUnion.msg_ClientLoginReq.Password[i]!=0;i++)
            strPasswd+= pAppLayer->MsgUnion.msg_ClientLoginReq.Password[i];

        QString sql = "select user_name,user_id,password from users where user_name = ? and password = ?;";
        query.prepare(sql);
        query.addBindValue(strUserName);
        query.addBindValue(strPasswd);

        if (true==query.exec())
        {
            if (query.next())
            {
                bool bOk = false;
                quint32 ncurrid = query.value(1).toUInt(&bOk);
                if (bOk==true)
                {
                    if (ncurrid>= (unsigned int)0x80000000 && ncurrid <=  (unsigned int)0xAFFFFFFF  )
                    {
                        reply.TextInfo[0] = 0;
                        reply.DoneCode = 0;
                        reply.UserID = ncurrid;
                        m_bLoggedIn = true;
                        m_bUUIDRecieved = true;
                        m_uuid = ncurrid;
                        m_pClientTable->regisitClientUUID(this);
                    }
                    else
                        strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
                }
                else
                    strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
            }
            else
            {
                // No such device
                strcpy(reply.TextInfo,"No such user or password.");
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
bool st_clientNodeAppLayer::Box2Svr_CorrectTime()
{
    //form Msgs
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_HostTimeCorrectRsp);
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
    pApp->header.MsgType = 0x1802;
    pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

    stMsg_HostTimeCorrectRsp & reply = pApp->MsgUnion.msg_HostTimeCorrectRsp;

    reply.DoneCode = 0;
    reply.TextInfo[0]= 0;

    QDateTime dtm = QDateTime::currentDateTimeUtc();

    reply.DateTime.Year = dtm.date().year();
    reply.DateTime.Month = dtm.date().month();
    reply.DateTime.Day = dtm.date().day();
    reply.DateTime.Hour = dtm.time().hour();
    reply.DateTime.Minute = dtm.time().minute();
    reply.DateTime.Second = dtm.time().second();
    //Send back
    emit evt_SendDataToClient(this->sock(),array);



    return reply.DoneCode==0?true:false;
}

bool st_clientNodeAppLayer::Box2Svr_UploadUserTable()
{
    const SMARTLINK_MSG_APP * pAppLayer =
            (const SMARTLINK_MSG_APP *)(
                ((const char *)(m_currentBlock.constData()))
                +sizeof(SMARTLINK_MSG)-1);
    //form Msgs
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_UploadUserListRsp);
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
    pApp->header.MsgType = 0x1803;
    pApp->header.MsgFmtVersion = m_current_app_header.header.MsgFmtVersion;

    stMsg_UploadUserListRsp & reply = pApp->MsgUnion.msg_UploadUserListRsp;

    reply.DoneCode = 3;
    strcpy(reply.TextInfo,"Unknown error");
    //Check the database, find current equipment info
    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());

    if (db.isValid()==true && db.isOpen()==true )
    {
        QSqlQuery query(db);

        QString sql = "select user_name,user_id,password from users where user_name = ? and password = ?;";
        query.prepare(sql);
//        query.addBindValue(strUserName);
//        query.addBindValue(strPasswd);

        if (true==query.exec())
        {
            if (query.next())
            {
                bool bOk = false;
                quint32 ncurrid = query.value(1).toUInt(&bOk);
                if (bOk==true)
                {
                    if (ncurrid>= (unsigned int)0x80000000 && ncurrid <=  (unsigned int)0xAFFFFFFF  )
                    {
                        reply.TextInfo[0] = 0;
                        reply.DoneCode = 0;

                    }
                    else
                        strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
                }
                else
                    strcpy(reply.TextInfo,"UserID Is Invalid.Accunt locked by svr");
            }
            else
            {
                // No such device
                strcpy(reply.TextInfo,"No such user or password.");
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
