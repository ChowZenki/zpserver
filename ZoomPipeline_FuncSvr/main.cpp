#include "zpmainframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ZPMainFrame w;
    w.show();
    int pp = a.exec();
    return pp;
}
