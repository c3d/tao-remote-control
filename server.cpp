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
#include "tao/tao_utf8.h"
#include "tao/module_api.h"
#include <QString>
#include <QTcpSocket>
#include <QTimer>


Server * Server::inst = NULL;

// Licensing
#define LICNAME "RemoteControl 1.02"
#define EVAL_MINUTES 5
#define STRINGIFY(s) #s
#define TOSTR(s) STRINGIFY(s)
static const QString licMsg("You have no valid license for " LICNAME ".\n"
                            "As a result, this module may only be used for "
                            TOSTR(EVAL_MINUTES)
                            " minutes after the application is started.\n");


using namespace Tao;


Server * Server::instance(const Tao::ModuleApi *tao, int port)
// ----------------------------------------------------------------------------
//    Return server instance or create it
// ----------------------------------------------------------------------------
{
    if (!inst)
        inst = new Server(tao, port);
    return inst;
}


Server::Server(const ModuleApi *tao, int port)
// ----------------------------------------------------------------------------
//    Start server
// ----------------------------------------------------------------------------
    : tao(tao)
{
    licensed = tao->checkImpressOrLicense(LICNAME);
    if (!licensed)
    {
        IFTRACE2(remotecontrol, lic)
            debug() << "Unlicensed\n";
        double expires = (EVAL_MINUTES*60 - tao->taoRunTime()) * 1000;
        if (expires < 0)
        {
            IFTRACE2(remotecontrol, lic)
                debug() << "Evaluation period expired, not listening.\n";
            return;
        }
        IFTRACE2(remotecontrol, lic)
            debug() << "Evaluation expires in " << expires << " ms\n";
        QTimer::singleShot(expires, this, SLOT(disconnectNoLicense()));
    }

    IFTRACE(remotecontrol)
        debug() << "Starting server\n";

    // Accept connection over IPv4 or IPv6
    QHostAddress address = QHostAddress::Any;

#if QT_VERSION < 0x050000  // Qt4
#  if defined(Q_OS_MACX) || defined(Q_OS_LINUX)
    // AnyIPv6 is v4 + v6, while Any is v4 only.
    address = QHostAddress::AnyIPv6;
#  else
    // With Qt4/Windows, AnyIPv6 is v6 only and Any is v4 only. There is
    // no simple way to do both v4 and v6, so keep v4 only.
#  endif
#endif

    if (!listen(address, port) && !listen(address, 0))
    {
        IFTRACE(remotecontrol)
            debug() << "Server failed to start\n";
        return;
    }
    IFTRACE(remotecontrol)
        debug() << "Bound to " << +serverAddress().toString()
                << ":" << serverPort() << "\n";
}


Server::~Server()
// ----------------------------------------------------------------------------
//   Stop server, close all sockets
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Stopping\n";
    close();
    foreach (ClientConnection *c, clients)
        delete c;
    clients.clear();
}


std::ostream & Server::debug()
// ----------------------------------------------------------------------------
//   Convenience method to log with a common prefix
// ----------------------------------------------------------------------------
{
    std::cerr << "[Server " << (void*)this << "] ";
    return std::cerr;
}


void Server::incomingConnection(int sd)
// ----------------------------------------------------------------------------
//   Accept new incoming connection
// ----------------------------------------------------------------------------
{
    if (!licensed && tao->taoRunTime() > (EVAL_MINUTES*60))
    {
        QTcpSocket socket;
        socket.setSocketDescriptor(sd);
        IFTRACE2(remotecontrol, lic)
            debug() << "Rejecting connection due to missing license\n";
        QString msg(licMsg);
        msg.replace(QChar('\n'), "\r\n");
        QByteArray ba(msg.toUtf8().constData());
        socket.write(ba);
        socket.flush();
        return;
    }

    IFTRACE(remotecontrol)
        debug() << "New connection accepted\n";

    ClientConnection * conn = new ClientConnection(sd);
    clients.insert(conn);
    conn->moveToThread(conn);
    connect(conn, SIGNAL(disconnected()),
            this, SLOT(onDisconnected()));
    conn->start();
}


void Server::onDisconnected()
// ----------------------------------------------------------------------------
//   Connection closed
// ----------------------------------------------------------------------------
{
    ClientConnection * conn = dynamic_cast<ClientConnection *>(sender());
    Q_ASSERT(conn);
    Q_ASSERT(clients.contains(conn));

    IFTRACE(remotecontrol)
        debug() << "Deleting connection\n";
    clients.remove(conn);
    delete conn;
}


QList<ClientConnection *> Server::clientConnections()
// ----------------------------------------------------------------------------
//   List all current connections from remote clients
// ----------------------------------------------------------------------------
{
    return clients.values();
}


void Server::disconnectNoLicense()
// ----------------------------------------------------------------------------
//   Close connection with all clients
// ----------------------------------------------------------------------------
{
    IFTRACE2(remotecontrol, lic)
        debug() << "Evaluation period expired, closing client connections\n";

    QString msg("\n" + licMsg + "Closing connection.\n");
    foreach (ClientConnection * client, clients.values())
    {
        QMetaObject::invokeMethod(client, "disconnect", Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
    }
}
