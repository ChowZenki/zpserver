#include "ZP_TcpServer.h"
#include <assert.h>
ZP_TcpServer::ZP_TcpServer(QObject *parent )
    : QTcpServer(parent)
{

}

void ZP_TcpServer::incomingConnection(qintptr socketDescriptor)
{
    emit evt_NewClientArrived(socketDescriptor);
}
