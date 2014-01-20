#include "zpmainframe.h"
#include "ui_zpmainframe.h"
#include <QDateTime>
#include <QDialog>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
using namespace ZPNetwork;
using namespace ZPTaskEngine;
ZPMainFrame::ZPMainFrame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ZPMainFrame)
{
    m_currentConffile = QCoreApplication::applicationFilePath()+".ini";
    ui->setupUi(this);

    //Create net engine
    m_netEngine = new zp_net_ThreadPool (4096);
    connect (m_netEngine,&zp_net_ThreadPool::evt_Message,this,&ZPMainFrame::on_evt_Message);
    connect (m_netEngine,&zp_net_ThreadPool::evt_SocketError,this,&ZPMainFrame::on_evt_SocketError);
    //Create TaskEngine
    m_taskEngine = new zp_pipeline(this);
    //Create Smartlink client table
    m_clientTable = new SmartLink::st_client_table (m_netEngine,m_taskEngine,this);

    m_nTimerId = startTimer(500);

    initUI();
    LoadSettings(m_currentConffile);
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

void ZPMainFrame::initUI()
{
    //Message Shown model
    m_pMsgModel = new QStandardItemModel(this);
    ui->listView_msg->setModel(m_pMsgModel);

    //Network listeners setting model
    m_pListenerModel = new QStandardItemModel(0,4,this);
    m_pListenerModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_pListenerModel->setHeaderData(1,Qt::Horizontal,tr("Addr"));
    m_pListenerModel->setHeaderData(2,Qt::Horizontal,tr("Port"));
    m_pListenerModel->setHeaderData(3,Qt::Horizontal,tr("SSL"));
    ui->tableView_listen->setModel(m_pListenerModel);

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
void ZPMainFrame::on_action_Start_Stop_triggered(bool setordel)
{
    if (setordel==true)
    {
        forkServer(m_currentConffile);
    }
    else
    {
        m_netEngine->RemoveAllAddresses();
        m_netEngine->RemoveClientTransThreads(-1,true);
        m_netEngine->RemoveClientTransThreads(-1,false);
        m_taskEngine->removeThreads(-1);

        while (m_netEngine->CanExit()==false || m_taskEngine->canClose()==false)
        {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            QThread::currentThread()->msleep(200);
        }

    }


}
void ZPMainFrame::forkServer(const QString & config_file)
{
    QSettings settings(config_file,QSettings::IniFormat);
    int nListeners = settings.value("settings/listeners",0).toInt();
    if (nListeners<0)
        nListeners = 0;
    if (nListeners>=1024)
        nListeners = 1024;
    //read listeners from ini
    m_netEngine->RemoveAllAddresses();
    while (m_netEngine->ListenerNames().size())
    {
        QThread::currentThread()->msleep(200);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    for (int i=0;i<nListeners;i++)
    {
        QString keyPrefix = QString ("listener%1/").arg(i);
        QString listen_name = settings.value(keyPrefix+"name",
                                             QString("Listener%1").arg(i)).toString();
        QString Address =   settings.value(keyPrefix+"address",
                QString("127.0.0.1")).toString();
        QHostAddress listen_address (Address) ;

        int nPort = settings.value(keyPrefix+"port",23456+i).toInt();
        bool bSSL = settings.value(keyPrefix+"ssl",i%2?true:false).toBool();
        if (listen_address.isNull()==true || nPort<1024 || nPort>=32768 )
            continue;

        m_netEngine->AddListeningAddress(listen_name,listen_address,nPort,bSSL);
    }
    //read thread config
    int nSSLThreads = settings.value("settings/nSSLThreads",4).toInt();
    int nPlainThreads = settings.value("settings/nPlainThreads",4).toInt();
    int nWorkingThreads = settings.value("settings/nWorkingThreads",8).toInt();

    int nDeltaSSL = m_netEngine->TransThreadNum(true) - nSSLThreads;
    if (nDeltaSSL>0)
        m_netEngine->RemoveClientTransThreads(nDeltaSSL,true);
    else if (nDeltaSSL<0)
        m_netEngine->AddClientTransThreads(-nDeltaSSL,true);

    int nDeltaPlain = m_netEngine->TransThreadNum(false) - nPlainThreads;
    if (nDeltaPlain>0)
        m_netEngine->RemoveClientTransThreads(nDeltaPlain,false);
    else if (nDeltaPlain<0)
        m_netEngine->AddClientTransThreads(-nDeltaPlain,false);

    int nDeltaWorking = m_taskEngine->threadsCount() - nWorkingThreads;
    if (nDeltaWorking>0)
        m_taskEngine->removeThreads(nDeltaWorking);
    else
        m_taskEngine->addThreads(-nDeltaWorking);

}

void ZPMainFrame::on_action_About_triggered()
{

    QApplication::aboutQt();
}
void ZPMainFrame::LoadSettings(const QString & config_file)
{
    QSettings settings(config_file,QSettings::IniFormat);
    int nListeners = settings.value("settings/listeners",0).toInt();
    if (nListeners<0)
        nListeners = 0;
    if (nListeners>=1024)
        nListeners = 1024;
    m_pListenerModel->removeRows(0,m_pListenerModel->rowCount());
    m_set_listenerNames.clear();
    //read listeners from ini
    int nInserted = 0;
    for (int i=0;i<nListeners;i++)
    {
        QString keyPrefix = QString ("listener%1/").arg(i);
        QString listen_name = settings.value(keyPrefix+"name",
                                             QString("Listener%1").arg(i)).toString();
        QString Address =   settings.value(keyPrefix+"address",
                QString("127.0.0.1")).toString();
        QHostAddress listen_address (Address) ;

        int nPort = settings.value(keyPrefix+"port",23456+i).toInt();
        bool bSSL = settings.value(keyPrefix+"ssl",i%2?true:false).toBool();
        if (listen_address.isNull()==true || nPort<1024 || nPort>=32768 )
            continue;
        if (m_set_listenerNames.contains(listen_name))
            continue;
        m_set_listenerNames.insert(listen_name);
        m_pListenerModel->insertRow(nInserted);
        m_pListenerModel->setData(m_pListenerModel->index(nInserted,0),listen_name);
        m_pListenerModel->setData(m_pListenerModel->index(nInserted,1),listen_address.toString());
        m_pListenerModel->setData(m_pListenerModel->index(nInserted,2),nPort);
        m_pListenerModel->setData(m_pListenerModel->index(nInserted,3),bSSL);
        nInserted++;
    }
    //read thread config
    int nSSLThreads = settings.value("settings/nSSLThreads",4).toInt();
    int nPlainThreads = settings.value("settings/nPlainThreads",4).toInt();
    int nWorkingThreads = settings.value("settings/nWorkingThreads",8).toInt();
    ui->dial_plain_trans_threads->setValue(nPlainThreads);
    ui->dial_ssl_trans_threads->setValue(nSSLThreads);
    ui->dial_task_working_threads->setValue(nWorkingThreads);
}


void ZPMainFrame::SaveSettings(const QString & config_file)
{
    QSettings settings(config_file,QSettings::IniFormat);
    int nListeners = m_pListenerModel->rowCount();
    settings.setValue("settings/listeners",nListeners);
    //save listeners to ini
    int nRealsave = 0;
    for (int i=0;i<nListeners;i++)
    {
        QString keyPrefix = QString ("listener%1/").arg(nRealsave);
        QString listen_name = m_pListenerModel->data(m_pListenerModel->index(i,0)).toString();
        settings.setValue(keyPrefix+"name",listen_name);
        QHostAddress listen_address (m_pListenerModel->data(m_pListenerModel->index(i,1)).toString()) ;
        settings.setValue(keyPrefix+"address",listen_address.toString());
        int nPort = m_pListenerModel->data(m_pListenerModel->index(i,2)).toInt();
        settings.setValue(keyPrefix+"port",nPort);
        bool bSSL = m_pListenerModel->data(m_pListenerModel->index(i,3)).toBool();
        settings.setValue(keyPrefix+"ssl",bSSL);
        if (listen_name.length()<=0 || nPort<1024 || nPort>32767)
            continue;
        nRealsave++;
    }
    settings.setValue("settings/listeners",nRealsave);

    //save thread config
    int nSSLThreads =  ui->dial_plain_trans_threads->value();
    settings.setValue("settings/nPlainThreads",nSSLThreads);
    int nPlainThreads = ui->dial_ssl_trans_threads->value();
    settings.setValue("settings/nSSLThreads",nPlainThreads);

    int nWorkingThreads = ui->dial_task_working_threads->value();
    settings.setValue("settings/nWorkingThreads",nWorkingThreads);



}
void ZPMainFrame::on_pushButton_addListener_clicked()
{
    QString name = ui->lineEdit_listenerName->text();
    QString Addr = ui->lineEdit_listenerAddr->text();
    QHostAddress address(Addr);
    if (address.isNull())
        address = QHostAddress(QHostAddress::Any);
    QString Port = ui->lineEdit_listenerPort->text();
    int nPort = Port.toInt();
    bool bSSL = ui->checkBox_listener_ssl->isChecked();
    if (m_set_listenerNames.contains(name))
    {
        QMessageBox::information(this,tr("Name Already Used."),tr("The listener name has been used."));
        return;
    }
    if (address.isNull()==true || nPort<1024 || nPort>32767)
    {
        QMessageBox::information(this,tr("Invalid Paraments."),tr("Address must be valid, Port between 1024 to 32767."));
        return;
    }
    int nRowCount = m_pListenerModel->rowCount();
    m_pListenerModel->insertRow(nRowCount);
    m_pListenerModel->setData(m_pListenerModel->index(nRowCount,0),name);
    m_pListenerModel->setData(m_pListenerModel->index(nRowCount,1),address.toString());
    m_pListenerModel->setData(m_pListenerModel->index(nRowCount,2),nPort);
    m_pListenerModel->setData(m_pListenerModel->index(nRowCount,3),bSSL);
    m_set_listenerNames.insert( name);
}

void ZPMainFrame::on_pushButton_delListener_clicked()
{
    QItemSelectionModel * ptr = ui->tableView_listen->selectionModel();
    QModelIndexList lst = ptr->selectedIndexes();
    QSet<int> nRows;
    foreach (QModelIndex item, lst)
        nRows.insert(item.row());
    int nct = 0;
    foreach (int row, nRows)
    {
        m_pListenerModel->removeRow(row - nct);
        nct++;
    }
}
void ZPMainFrame::on_pushButton_listerner_apply_clicked()
{
    SaveSettings(m_currentConffile);
    //forkServer(m_currentConffile);
}
void ZPMainFrame::on_pushButton_threadsApply_clicked()
{
    SaveSettings(m_currentConffile);
    //forkServer(m_currentConffile);
}
void ZPMainFrame::on_actionReload_config_file_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Conf file"),QCoreApplication::applicationDirPath(),
                                 tr("Ini files(*.ini)"));
    if (filename.length()>0)
    {
        SaveSettings(m_currentConffile);
        m_currentConffile = filename;
        LoadSettings(m_currentConffile);
        forkServer(m_currentConffile);
    }
}
