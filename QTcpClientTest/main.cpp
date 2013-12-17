#include "qtcpclienttest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpClientTest w;
    w.show();
    return a.exec();
}
