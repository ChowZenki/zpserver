#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include "qghtcpclient.h"
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
public slots:
    void new_data_recieved();
    void on_client_trasferred(qint64);
    void on_client_connected();
    void on_client_disconnected();
    void displayError(QAbstractSocket::SocketError);
    void displayMessage(const QString &str);

    void on_pushButton_connect_clicked();
};

#endif // MAINDIALOG_H
