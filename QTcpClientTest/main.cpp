#include "qtcpclienttest.h"
#include <QApplication>
#include <time.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	srand(time(0));
	QApplication a(argc, argv);
	QTcpClientTest w;
	w.show();
	return a.exec();
}
