// ****************************************************************************
//  server.cpp                                                     Tao project
// ****************************************************************************
//
//   File Description:
//
//    TCP server accepts commands from remote clients. Commands are handed
//    to Hook objects for later execution in the Tao document.
//
//
//
//
//
//
//
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************


#include "server.h"
#include "client_connection.h"
#include "base.h"
#include <QTcpSocket>


Server * Server::inst = NULL;


inline std::string operator +(QString s)
// ----------------------------------------------------------------------------
//   Convert QString to std::string
// ----------------------------------------------------------------------------
{
    return std::string(s.toUtf8().constData());
}


Server * Server::instance(int port)
// ----------------------------------------------------------------------------
//    Return server instance or create it
// ----------------------------------------------------------------------------
{
    if (!inst)
        inst = new Server(port);
    if (!inst->isListening())
        return NULL;
    return inst;
}


Server::Server(int port)
// ----------------------------------------------------------------------------
//    Start server
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Starting server\n";

    if (!listen(QHostAddress::Any, port) &&
        !listen(QHostAddress::Any, 0))
    {
        IFTRACE(remotecontrol)
            debug() << "Server failed to start\n";
        return;
    }
    IFTRACE(remotecontrol)
        debug() << "Bound to " << +serverAddress().toString()
                << ":" << serverPort() << "\n";
    connect(this, SIGNAL(newConnection()),
            this, SLOT(onNewConnection()));
}


std::ostream & Server::debug()
// ----------------------------------------------------------------------------
//   Convenience method to log with a common prefix
// ----------------------------------------------------------------------------
{
    std::cerr << "[Server " << (void*)this << "] ";
    return std::cerr;
}


void Server::onNewConnection()
// ----------------------------------------------------------------------------
//   Accept new incoming connection
// ----------------------------------------------------------------------------
{
    QTcpSocket * socket = nextPendingConnection();
    Q_ASSERT(socket);

    IFTRACE(remotecontrol)
        debug() << "New connection accepted\n";

    ClientConnection * conn = new ClientConnection(socket);
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(onDisconnected()));
    clients[socket] = conn;

}


void Server::onDisconnected()
// ----------------------------------------------------------------------------
//   Accept new incoming connection
// ----------------------------------------------------------------------------
{
    QTcpSocket * socket = dynamic_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket);
    Q_ASSERT(clients.count(socket));

    IFTRACE(remotecontrol)
        debug() << "Connection closed\n";

    ClientConnection * conn = clients[socket];
    clients.erase(socket);
    conn->deleteLater();
}
