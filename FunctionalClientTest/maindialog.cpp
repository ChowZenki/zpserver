#include "maindialog.h"
#include "ui_maindialog.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_message.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_msg_applayer.h"
#include <QSettings>
#include <time.h>
#include <QMessageBox>
#include <string.h>
using namespace SmartLink;
MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    nTimer = startTimer(100);
    ui->listView_msg->setModel(&model);
    client = new QGHTcpClient (this);
    connect(client, SIGNAL(readyRead()),this, SLOT(new_data_recieved()));
    connect(client, SIGNAL(connected()),this, SLOT(on_client_connected()));
    connect(client, SIGNAL(disconnected()),this,SLOT(on_client_disconnected()));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(on_client_trasferred(qint64)));

    m_currentReadOffset = 0;
    m_currentMessageSize = 0;


    QSettings settings("goldenhawking club","FunctionalClientTest",this);
    ui->lineEdit_ip->setText(settings.value("settings/ip","localhost").toString());
    ui->lineEdit_Port->setText(settings.value("settings/port","23456").toString());
    ui->plainTextEdit_boxSerialNum->setPlainText(settings.value("settings/box2svr_insid","Temporary Equip_id for test only, by goldenhawking@163.com.64Bts").toString());
    ui->lineEdit_username->setText(settings.value("settings/client2svr_username","debug").toString());
    ui->lineEdit_password->setText(settings.value("settings/client2svr_password","debug").toString());
}

MainDialog::~MainDialog()
{
    delete ui;
}
void MainDialog::on_client_trasferred(qint64 dtw)
{
    QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
    if (pSock)
    {
        displayMessage(QString("client %1 Transferrd %2 bytes.").arg((quintptr)pSock).arg(dtw));
    }

}
void MainDialog::on_client_connected()
{
    QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
    if (pSock)
    {
        displayMessage(QString("client %1 connected.").arg((quintptr)pSock));
        ui->pushButton_connect->setEnabled(false);
    }

}
void MainDialog::on_client_disconnected()
{
    QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
    if (pSock)
    {
        displayMessage(QString("client %1 disconnected.").arg((quintptr)pSock));
        ui->pushButton_connect->setEnabled(true);
        pSock->abort();
    }
}
void MainDialog::displayError(QAbstractSocket::SocketError /*err*/)
{
    QGHTcpClient * sock = qobject_cast<QGHTcpClient *> (sender());
    if (sock)
        displayMessage(QString("client %1 error msg:").arg((quintptr)sock)+sock->errorString());
}
void MainDialog::new_data_recieved()
{
    QTcpSocket * pSock = qobject_cast<QTcpSocket*>(sender());
    if (pSock)
    {
        QByteArray block =pSock->readAll();
        if (block.isEmpty()==false && block.size()>0)
        {

            do
            {
                m_currentReadOffset = filter_message(block,m_currentReadOffset);
            }
            while (m_currentReadOffset < block.size());
            m_currentReadOffset = 0;
        }
    }
}
void MainDialog::displayMessage(const QString &str)
{
    model.insertRow(0,new QStandardItem(str));
    while (model.rowCount()>=256)
        model.removeRow(model.rowCount()-1);
}
void MainDialog::saveIni()
{
    QSettings settings("goldenhawking club","FunctionalClientTest",this);
    settings.setValue("settings/ip", ui->lineEdit_ip->text());
    settings.setValue("settings/port", ui->lineEdit_Port->text());
    settings.setValue("settings/box2svr_insid", ui->plainTextEdit_boxSerialNum->toPlainText());
    settings.setValue("settings/client2svr_username", ui->lineEdit_username->text());
    settings.setValue("settings/client2svr_password", ui->lineEdit_password->text());
}

void MainDialog::timerEvent(QTimerEvent * evt)
{
    static int nCount = 0;
    if (evt->timerId()==nTimer)
    {
        nCount++;
        if (nCount % 200 == 0 && client->isOpen()==true)
        {
            //send heart-beating
            QByteArray array(sizeof(SMARTLINK_HEARTBEATING),0);
            char * ptr = array.data();
            SMARTLINK_HEARTBEATING * pMsg = (SMARTLINK_HEARTBEATING *)ptr;
            pMsg->Mark = 0xBEBE;
            pMsg->tmStamp = time(0)&0x00ffff;
            //3/10 possibility to send a data block to server
            client->SendData(array);

        }
    }
}
void MainDialog::on_pushButton_connect_clicked()
{
    saveIni();
    client->connectToHost(ui->lineEdit_ip->text(),ui->lineEdit_Port->text().toUShort());
}
void MainDialog::on_pushButton_regisit_clicked()
{
    saveIni();
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_HostRegistReq);
    QByteArray array(sizeof(SMARTLINK_MSG) + nMsgLen - 1,0);
    char * ptr = array.data();
    SMARTLINK_MSG * pMsg = (SMARTLINK_MSG *)ptr;
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );
    pMsg->Mark = 0x55AA;
    pMsg->version = 1;
    pMsg->SerialNum = 0;
    pMsg->Priority = 1;
    pMsg->Reserved1 = 0;
    pMsg->source_id = (quint32)((quint64)(0xffffffff) & 0xffffffff );

    pMsg->destin_id = (quint32)((quint64)(0x00000001) & 0xffffffff );;

    pMsg->data_length = nMsgLen;
    pMsg->Reserved2 = 0;


    pApp->header.AskID = 0x01;
    pApp->header.MsgType = 0x1000;
    pApp->header.MsgFmtVersion = 0x01;
    QString strSerial = ui->plainTextEdit_boxSerialNum->toPlainText();
    std::string strStdSerial = strSerial.toStdString();
    const char * pSrc = strStdSerial.c_str();
    int nMaxLen = strStdSerial.length();
    for (int i=0;i<64;i++)
        pApp->MsgUnion.msg_HostRegistReq.HostSerialNum[i] =
                i<nMaxLen?pSrc[i]:0;
    //3/10 possibility to send a data block to server
    client->SendData(array);
}

void MainDialog::on_pushButton_Login_clicked()
{
    saveIni();
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_HostLogonReq);
    QByteArray array(sizeof(SMARTLINK_MSG) + nMsgLen - 1,0);
    char * ptr = array.data();
    SMARTLINK_MSG * pMsg = (SMARTLINK_MSG *)ptr;
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );
    pMsg->Mark = 0x55AA;
    pMsg->version = 1;
    pMsg->SerialNum = 0;
    pMsg->Priority = 1;
    pMsg->Reserved1 = 0;
    pMsg->source_id = (quint32)((quint64)(0xffffffff) & 0xffffffff );

    pMsg->destin_id = (quint32)((quint64)(0x00000001) & 0xffffffff );;

    pMsg->data_length = nMsgLen;
    pMsg->Reserved2 = 0;


    pApp->header.AskID = 0x01;
    pApp->header.MsgType = 0x1001;
    pApp->header.MsgFmtVersion = 0x01;
    QString strSerial = ui->plainTextEdit_boxSerialNum->toPlainText();
    std::string strStdSerial = strSerial.toStdString();
    const char * pSrc = strStdSerial.c_str();
    int nMaxLen = strStdSerial.length();
    for (int i=0;i<64;i++)
        pApp->MsgUnion.msg_HostLogonReq.HostSerialNum[i] =
                i<nMaxLen?pSrc[i]:0;
    pApp->MsgUnion.msg_HostLogonReq.ID = ui->lineEdit_boxid->text().toUInt();

    //3/10 possibility to send a data block to server
    client->SendData(array);
}
void MainDialog::on_pushButton_clientLogin_clicked()
{
    saveIni();
    quint16 nMsgLen = sizeof(SMARTLINK_MSG_APP::tag_app_layer_header)
            +sizeof(stMsg_ClientLoginReq);
    QByteArray array(sizeof(SMARTLINK_MSG) + nMsgLen - 1,0);
    char * ptr = array.data();
    SMARTLINK_MSG * pMsg = (SMARTLINK_MSG *)ptr;
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );
    pMsg->Mark = 0x55AA;
    pMsg->version = 1;
    pMsg->SerialNum = 0;
    pMsg->Priority = 1;
    pMsg->Reserved1 = 0;
    pMsg->source_id = (quint32)((quint64)(0xffffffff) & 0xffffffff );

    pMsg->destin_id = (quint32)((quint64)(0x00000001) & 0xffffffff );;

    pMsg->data_length = nMsgLen;
    pMsg->Reserved2 = 0;


    pApp->header.AskID = 0x01;
    pApp->header.MsgType = 0x3000;
    pApp->header.MsgFmtVersion = 0x01;
    QString strUserName = ui->lineEdit_username->text();
    QString strPassWd = ui->lineEdit_password->text();

    std::string strStdUserName = strUserName.toStdString();
    const char * pSrc = strStdUserName.c_str();
    int nMaxLen = strStdUserName.length();
    for (int i=0;i<32;i++)
        pApp->MsgUnion.msg_ClientLoginReq.UserName[i] =
                i<nMaxLen?pSrc[i]:0;

    std::string strStdPassword = strPassWd.toStdString();
    pSrc = strStdPassword.c_str();
    nMaxLen = strStdPassword.length();
    for (int i=0;i<32;i++)
        pApp->MsgUnion.msg_ClientLoginReq.Password[i] =
                i<nMaxLen?pSrc[i]:0;

    pApp->MsgUnion.msg_ClientLoginReq.ClientVersion = 0;

    //3/10 possibility to send a data block to server
    client->SendData(array);
}

//!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
//!return bytes Used.
int MainDialog::filter_message(const QByteArray & block, int offset)
{
    const int blocklen = block.length();
    while (blocklen>offset)
    {
        const char * dataptr = block.constData();

        //Recieve First 2 byte
        while (m_currentMessageSize<2 && blocklen>offset )
        {
            m_currentBlock.push_back(dataptr[offset++]);
            m_currentMessageSize++;
        }
        if (m_currentMessageSize < 2) //First 2 byte not complete
            continue;

        if (m_currentMessageSize==2)
        {
            const char * headerptr = m_currentBlock.constData();
            memcpy((void *)&m_currentHeader,headerptr,2);
        }

        const char * ptrCurrData = m_currentBlock.constData();
        //Heart Beating
        if (m_currentHeader.Mark == 0xBEBE)
        {
            while (m_currentMessageSize< sizeof(SMARTLINK_HEARTBEATING) && blocklen>offset )
            {
                m_currentBlock.push_back(dataptr[offset++]);
                m_currentMessageSize++;
            }
            if (m_currentMessageSize < sizeof(SMARTLINK_HEARTBEATING)) //Header not completed.
                continue;
            const SMARTLINK_HEARTBEATING * ptr = (const SMARTLINK_HEARTBEATING *)m_currentBlock.constData();
            displayMessage(tr("Recieved Heart-beating msg sended %1 sec(s) ago.").
                           arg((time(0)&0x00ffff)-(ptr->tmStamp)));
            //This Message is Over. Start a new one.
            m_currentMessageSize = 0;
            m_currentBlock = QByteArray();

            continue;
        }
        else if (m_currentHeader.Mark == 0x55AA)
            //Trans Message
        {
            while (m_currentMessageSize< sizeof(SMARTLINK_MSG)-1 && blocklen>offset)
            {
                m_currentBlock.push_back(dataptr[offset++]);
                m_currentMessageSize++;
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
                    while (bitLeft>0 && blocklen>offset)
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
                    m_currentBlock = QByteArray();
                    continue;
                }
            }
            else
            {
                if (block.length()>offset)
                {
                    qint32 bitLeft = m_currentHeader.data_length + sizeof(SMARTLINK_MSG) - 1
                            -m_currentMessageSize ;
                    while (bitLeft>0 && blocklen>offset)
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
                    m_currentBlock = QByteArray();
                    continue;
                }
            } // end if there is more bytes to append
        } //end deal trans message
        else
        {
            displayMessage(tr("Client Send a unknown start Header %1 %2. Close client immediately.")
                             .arg((int)(ptrCurrData[0])).arg((int)(ptrCurrData[1])));
            m_currentMessageSize = 0;
            m_currentBlock = QByteArray();
            offset = blocklen;

            this->client->abort();
        }
    } // end while block len > offset

    return offset;
}
//!deal current message
int MainDialog::deal_current_message_block()
{
    //The bytes left to recieve.
    qint32 bytesLeft = m_currentHeader.data_length + sizeof(SMARTLINK_MSG) - 1
            -m_currentMessageSize ;
    if (bytesLeft)
        return 0;

    char * ptr = m_currentBlock.data();
    SMARTLINK_MSG_APP * pApp = (SMARTLINK_MSG_APP *)(((unsigned char *)
                                                      (ptr))+sizeof(SMARTLINK_MSG)-1
                                                     );

    if (pApp->header.MsgType==0x1800)
    {
        ui->lineEdit_boxid->setText(QString("%1").arg(pApp->MsgUnion.msg_HostRegistRsp.ID));
        displayMessage(tr("Res = %1, ID = %2, Text = %3")
                       .arg(pApp->MsgUnion.msg_HostRegistRsp.DoneCode)
                       .arg(pApp->MsgUnion.msg_HostRegistRsp.ID)
                       .arg(pApp->MsgUnion.msg_HostRegistRsp.TextInfo)
                       );
    }
    else if (pApp->header.MsgType==0x1801)
    {
        if (pApp->MsgUnion.msg_HostLogonRsp.DoneCode==0)
            QMessageBox::information(this,tr("Succeed!"),tr("Log in succeed!"));
        else
            QMessageBox::information(this,tr("Failed!"),tr("Log in Failed!"));
        displayMessage(tr("Res = %1, Text = %2")
                       .arg(pApp->MsgUnion.msg_HostLogonRsp.DoneCode)
                       .arg(pApp->MsgUnion.msg_HostLogonRsp.TextInfo)
                       );
    }
    else if (pApp->header.MsgType==0x3800)
    {
        if (pApp->MsgUnion.msg_ClientLoginRsp.DoneCode==0)
            QMessageBox::information(this,tr("Succeed!"),tr("Log in succeed!"));
        else
            QMessageBox::information(this,tr("Failed!"),tr("Log in Failed!"));
        displayMessage(tr("Res = %1, Text = %2")
                       .arg(pApp->MsgUnion.msg_ClientLoginRsp.DoneCode)
                       .arg(pApp->MsgUnion.msg_ClientLoginRsp.TextInfo)
                       );
    }
    m_currentBlock = QByteArray();


    return 0;
}
