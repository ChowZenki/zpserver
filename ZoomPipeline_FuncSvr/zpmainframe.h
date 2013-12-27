#ifndef ZPMAINFRAME_H
#define ZPMAINFRAME_H

#include <QMainWindow>
#include "network/zp_net_threadpool.h"
#include "pipeline/zp_pipeline.h"
#include "smartlink/st_client_table.h"
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
    void timerEvent(QTimerEvent *);
private:
    Ui::ZPMainFrame *ui;
    ZPNetwork::zp_net_ThreadPool * m_netEngine;
    ZPTaskEngine::zp_pipeline * m_taskEngine;
    SmartLink::st_client_table * m_clientTable;
    int m_nTimerId;
public slots:
    //These Message is nessery.-------------------------------------
    void on_evt_Message(const QString &);
    //The socket error message
    void on_evt_SocketError(QObject * senderSock ,QAbstractSocket::SocketError socketError);
};

#endif // ZPMAINFRAME_H
