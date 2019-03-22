// *****************************************************************************
// server.cpp                                                      Tao3D project
// *****************************************************************************
//
// File description:
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
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2012-2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2012-2013, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012-2015,2019, Christophe de Dinechin <christophe@dinechin.org>
// (C) 2012-2013, Jérôme Forissier <jerome@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************


#include "server.h"
#include "client_connection.h"
#include "base.h"
#include "tao/tao_utf8.h"
#include "tao/module_api.h"
#include <QString>
#include <QTcpSocket>
#include <QTimer>


Server * Server::inst = NULL;

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


void Server::incomingConnection(Server::handle_t sd)
// ----------------------------------------------------------------------------
//   Accept new incoming connection
// ----------------------------------------------------------------------------
{
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
    XL_ASSERT(conn);
    XL_ASSERT(clients.contains(conn));

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
