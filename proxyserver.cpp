#include "proxyserver.h"

ProxyServer::ProxyServer(uint localPort, QString remote, QObject *parent) : QObject(parent)
{
    _server = new QTcpServer;
}

ProxyServer::~ProxyServer()
{

}

bool ProxyServer::listen()
{
    return true;
}
