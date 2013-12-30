#include "zpmainframe.h"
#include "ui_zpmainframe.h"
#include <functional>
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

    m_netEngine->AddListeningAddress("10302",QHostAddress::Any,10302);
    m_netEngine->AddListeningAddress("10202",QHostAddress::Any,10202);
    m_netEngine->AddClientTransThreads(4);

    //Create TaskEngine
    m_taskEngine = new zp_pipeline(this);
    m_taskEngine->addThreads(4);


    m_clientTable = new SmartLink::st_client_table (m_netEngine,m_taskEngine,this);



    m_nTimerId = startTimer(500);

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
        //_sleep(100);
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
void  ZPMainFrame::on_evt_Message(const QString &)
{

}

//The socket error message
void  ZPMainFrame::on_evt_SocketError(QObject * /*senderSock*/ ,QAbstractSocket::SocketError/* socketError*/)
{

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
