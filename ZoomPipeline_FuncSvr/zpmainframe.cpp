#include "zpmainframe.h"
#include "ui_zpmainframe.h"
#include <QDateTime>
using namespace ZPNetwork;
using namespace ZPTaskEngine;
ZPMainFrame::ZPMainFrame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ZPMainFrame)
{
    ui->setupUi(this);

    //Create net engine
    m_netEngine = new zp_net_ThreadPool (4096);

    connect (m_netEngine,&zp_net_ThreadPool::evt_Message,this,&ZPMainFrame::on_evt_Message);
    connect (m_netEngine,&zp_net_ThreadPool::evt_SocketError,this,&ZPMainFrame::on_evt_SocketError);


    m_netEngine->AddListeningAddress("23456_Plain",QHostAddress::Any,23456,false);
    m_netEngine->AddListeningAddress("23457_SSL",QHostAddress::Any,23457,true);
    m_netEngine->AddClientTransThreads(4,false);
    m_netEngine->AddClientTransThreads(4,true);

    //Create TaskEngine
    m_taskEngine = new zp_pipeline(this);
    m_taskEngine->addThreads(4);


    m_clientTable = new SmartLink::st_client_table (m_netEngine,m_taskEngine,this);



    m_nTimerId = startTimer(500);

    m_pMsgModel = new QStandardItemModel(this);
    ui->listView_msg->setModel(m_pMsgModel);

}

ZPMainFrame::~ZPMainFrame()
{
    m_netEngine->RemoveAllAddresses();
    m_netEngine->KickAllClients();
    m_netEngine->DeactiveImmediately();

    m_taskEngine->removeThreads(-1);

    while (m_netEngine->CanExit()==false || m_taskEngine->canClose()==false)
    {
        QCoreApplication::processEvents();
        QThread::currentThread()->msleep(200);
    }

    delete ui;
}

void ZPMainFrame::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
//These Message is nessery.-------------------------------------
void  ZPMainFrame::on_evt_Message(const QString & strMsg)
{
    QDateTime dtm = QDateTime::currentDateTime();
    QString msg = dtm.toString("yyyy-MM-dd HH:mm:ss.zzz") + " " + strMsg;
    int nrows = m_pMsgModel->rowCount();
    m_pMsgModel->insertRow(0,new QStandardItem(msg));
    while (nrows-- > 16384)
        m_pMsgModel->removeRow(m_pMsgModel->rowCount()-1);
}

//The socket error message
void  ZPMainFrame::on_evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError)
{
    QDateTime dtm = QDateTime::currentDateTime();
    QString msg = dtm.toString("yyyy-MM-dd HH:mm:ss.zzz") + " " + QString("SockError %1 with code %2")
            .arg((quint64)senderSock).arg((quint32)socketError);
    int nrows = m_pMsgModel->rowCount();
    m_pMsgModel->insertRow(0,new QStandardItem(msg));
    while (nrows-- > 16384)
        m_pMsgModel->removeRow(m_pMsgModel->rowCount()-1);

}


void  ZPMainFrame::timerEvent(QTimerEvent * e)
{
    if (e->timerId()==m_nTimerId)
    {
        //recording net status
        QString str_msg;
        QStringList lstListeners = m_netEngine->ListenerNames();
        str_msg += tr("Current Listen Threads: %1\n").arg(lstListeners.size());
        for (int i=0;i<lstListeners.size();i++)
            str_msg += tr("\tListen Threads %1: %2\n").arg(i+1).arg(lstListeners.at(i));
        int nClientThreads = m_netEngine->TransThreadNum();

        str_msg += tr("Current Trans Threads: %1\n").arg(nClientThreads);
        for (int i=0;i<nClientThreads;i++)
            str_msg += tr("\tTrans Threads %1 hold %2 Client Sockets.\n").arg(i+1).arg(m_netEngine->totalClients(i));

        //recording task status
        str_msg += tr("Current Task Threads: %1\n").arg(m_taskEngine->threadsCount());
        str_msg += tr("Current Task Payload: %1\n").arg(m_taskEngine->payload());
        str_msg += tr("Current Task Idle Threads: %1\n").arg(m_taskEngine->idleThreads());
        ui->plainTextEdit_status_net->setPlainText(str_msg);
    }
}
