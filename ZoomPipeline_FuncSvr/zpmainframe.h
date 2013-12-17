#ifndef ZPMAINFRAME_H
#define ZPMAINFRAME_H

#include <QMainWindow>
#include "network/zp_net_threadpool.h"
namespace Ui {
class ZPMainFrame;
}

class ZPMainFrame : public QMainWindow
{
    Q_OBJECT

public:
    explicit ZPMainFrame(QWidget *parent = 0);
    ~ZPMainFrame();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ZPMainFrame *ui;
    zp_net_ThreadPool * m_pool;
public slots:
    //These Message is nessery.-------------------------------------
    void on_evt_Message(const QString &);
    //The socket error message
    void on_evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
    //this event indicates new client connected.
    void on_evt_NewClientConnected(QObject * /*clientHandle*/);
    //this event indicates a client disconnected.
    void on_evt_ClientDisconnected(QObject * /*clientHandle*/);
    //some data arrival
    void on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
    //a block of data has been successfuly sent
    void on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);
};

#endif // ZPMAINFRAME_H
