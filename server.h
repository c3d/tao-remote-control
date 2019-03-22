#ifndef SERVER_H
#define SERVER_H
// *****************************************************************************
// server.h                                                        Tao3D project
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
// (C) 2012-2014,2019, Christophe de Dinechin <christophe@dinechin.org>
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


#include "tao/module_api.h"
#include <QList>
#include <QSet>
#include <QTcpServer>
#include <iostream>


class ClientConnection;

class Server : public QTcpServer
// ----------------------------------------------------------------------------
//    Command server is a singleton
// ----------------------------------------------------------------------------
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    typedef qintptr handle_t;
#else
    typedef int handle_t;
#endif

public:
    static Server *     instance(const Tao::ModuleApi *tao, int port = 6560);
    static bool         started() { return inst; }
    static void         destroy() { if (inst) { delete inst; inst = NULL; } }

public:
    QList<ClientConnection *> clientConnections();

protected:
    Server(const Tao::ModuleApi *tao, int port);
    virtual ~Server();

protected:
    virtual void        incomingConnection(handle_t handle);

protected:
    std::ostream &      debug();

private slots:
    void                onDisconnected();

private:
    typedef QSet<ClientConnection *> client_set;
    const Tao::ModuleApi * tao;
    bool                licensed;
    client_set          clients;

private:
    static Server *     inst;
};

#endif // SERVER_H
