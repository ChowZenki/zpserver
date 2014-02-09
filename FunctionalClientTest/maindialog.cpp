#include "maindialog.h"
#include "ui_maindialog.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_message.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_msg_applayer.h"
#include <QSettings>
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

    QSettings settings("goldenhawking club","FunctionalClientTest",this);
    ui->lineEdit_ip->setText(settings.value("ip","localhost").toString());
    ui->lineEdit_Port->setText(settings.value("port","23456").toString());

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
        QByteArray array =pSock->readAll();
        //in this example, we just do nothing but to display the byte size.
        displayMessage(QString("client %1 Recieved %2 bytes.").arg((quintptr)pSock).arg(array.size()));
    }
}
void MainDialog::displayMessage(const QString &str)
{
    model.insertRow(0,new QStandardItem(str));
    while (model.rowCount()>=256)
        model.removeRow(model.rowCount()-1);
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
            pMsg->tmStamp = 0;
            //3/10 possibility to send a data block to server
            client->SendData(array);

        }
    }
}
void MainDialog::on_pushButton_connect_clicked()
{
    if (client->isOpen()==false)
        client->connectToHost(ui->lineEdit_ip->text(),ui->lineEdit_Port->text().toUShort());
    else
        client->disconnectFromHost();
}
