#include "zpmainframe.h"
#include "ui_zpmainframe.h"
using namespace ZPNetwork;
ZPMainFrame::ZPMainFrame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ZPMainFrame)
{
    ui->setupUi(this);

    m_nTimerId = startTimer(500);
    m_pool = new zp_net_ThreadPool (4096);

    connect (m_pool,&zp_net_ThreadPool::evt_Message,this,&ZPMainFrame::on_evt_Message);
    connect (m_pool,&zp_net_ThreadPool::evt_SocketError,this,&ZPMainFrame::on_evt_SocketError);
    connect (m_pool,&zp_net_ThreadPool::evt_NewClientConnected,this,&ZPMainFrame::on_evt_NewClientConnected);
    connect (m_pool,&zp_net_ThreadPool::evt_ClientDisconnected,this,&ZPMainFrame::on_evt_ClientDisconnected);
    connect (m_pool,&zp_net_ThreadPool::evt_Data_recieved,this,&ZPMainFrame::on_evt_Data_recieved);
    connect (m_pool,&zp_net_ThreadPool::evt_Data_transferred,this,&ZPMainFrame::on_evt_Data_transferred);

    m_pool->AddListeningAddress("10302",QHostAddress::Any,10302);
    m_pool->AddListeningAddress("10202",QHostAddress::Any,10202);
    m_pool->AddClientTransThreads(4);
}

ZPMainFrame::~ZPMainFrame()
{
    m_pool->RemoveAllAddresses();
    m_pool->KickAllClients();
    m_pool->DeactiveImmediately();
    while (m_pool->CanExit()==false)
    {
        QCoreApplication::processEvents();
        _sleep(100);
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
void  ZPMainFrame::on_evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError)
{

}

//this event indicates new client connected.
void  ZPMainFrame::on_evt_NewClientConnected(QObject * /*clientHandle*/)
{

}

//this event indicates a client disconnected.
void  ZPMainFrame::on_evt_ClientDisconnected(QObject * /*clientHandle*/)
{

}

//some data arrival
void  ZPMainFrame::on_evt_Data_recieved(QObject *  clientHandle,const QByteArray & datablock )
{
    this->m_pool->SendDataToClient(clientHandle,datablock);
}

//a block of data has been successfuly sent
void  ZPMainFrame::on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/)
{

}

void  ZPMainFrame::timerEvent(QTimerEvent * e)
{
    if (e->timerId()==m_nTimerId)
    {
        QString strNetStatus;
        QStringList lstListeners = m_pool->ListenerNames();
        strNetStatus += tr("Current Listen Threads: %1\n").arg(lstListeners.size());
        for (int i=0;i<lstListeners.size();i++)
            strNetStatus += tr("\tListen Threads %1: %2\n").arg(i+1).arg(lstListeners.at(i));
        int nClientThreads = m_pool->TransThreadNum();

        strNetStatus += tr("Current Trans Threads: %1\n").arg(nClientThreads);
        for (int i=0;i<nClientThreads;i++)
            strNetStatus += tr("\tTrans Threads %1 hold %2 Client Sockets.\n").arg(i+1).arg(m_pool->totalClients(i));
        ui->plainTextEdit_status_net->setPlainText(strNetStatus);
    }
}
