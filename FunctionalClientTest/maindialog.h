#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QByteArray>
#include "qghtcpclient.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_message.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_msg_applayer.h"
using namespace SmartLink;
namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();
    virtual void timerEvent(QTimerEvent * evt);
private:
    Ui::MainDialog *ui;
    QGHTcpClient * client;
    QStandardItemModel model;
    int nTimer;
    void saveIni();
    //!Message Dealers
protected:
    //!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
    //!return bytes Used.
    int filter_message(const QByteArray &, int offset);
    int deal_current_message_block();

    //data items
protected:
    //The current Read Offset, from m_list_RawData's beginning
    int m_currentReadOffset;
    //Current Message Offset, according to m_currentHeader
    int m_currentMessageSize;
    //Current un-procssed message block.for large blocks,
    //this array will be re-setted as soon as some part of data has been
    //dealed, eg, send a 200MB block, the 200MB data will be splitted into pieces
    QByteArray m_currentBlock;
    //current Header
    SMARTLINK_MSG m_currentHeader;
    SMARTLINK_MSG_APP m_current_app_header;

public slots:
    void new_data_recieved();
    void on_client_trasferred(qint64);
    void on_client_connected();
    void on_client_disconnected();
    void displayError(QAbstractSocket::SocketError);
    void displayMessage(const QString &str);

    void on_pushButton_connect_clicked();
    void on_pushButton_regisit_clicked();
    void on_pushButton_Login_clicked();
};

#endif // MAINDIALOG_H
