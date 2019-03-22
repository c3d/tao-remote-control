// *****************************************************************************
// remote_control.cpp                                              Tao3D project
// *****************************************************************************
//
// File description:
//
//    Functions control the Tao process remotely.
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012,2019, Christophe de Dinechin <christophe@dinechin.org>
// (C) 2012, Jérôme Forissier <jerome@taodyne.com>
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

#include "remote_control.h"
#include "hook.h"
#include "hook_manager.h"
#include "server.h"
#include "client_connection.h"
#include "tao/module_api.h"
#include "tao/tao_utf8.h"

using namespace Tao;


// Pointer to Tao functions
const Tao::ModuleApi *tao = NULL;



XL::Tree_p remoteControlHook(XL::Context *context, XL::Tree_p self,
                             int id, int port)
// ----------------------------------------------------------------------------
//    Insertion point for arbitrary commands from remote client
// ----------------------------------------------------------------------------
{
    // First call instantiates server
    (void)Server::instance(tao, port);

    Hook * hook = HookManager::instance()->hook(id);
    hook->exec(context, self);
    tao->refreshOn(hook->refreshEvent, -1.0);

    return XL::xl_false;
}


struct Once : XL::Info {};

XL::Tree_p remoteControlWriteln(XL::Tree_p self, int id, text msg, bool once)
// ----------------------------------------------------------------------------
//    Sent msg to all clients connected to hook # id
// ----------------------------------------------------------------------------
{
    if (!Server::started())
        return XL::xl_false;

    if (once)
    {
        if (self->GetInfo<Once>())
            return XL::xl_false;
        self->SetInfo<Once>(new Once);
    }

    msg += "\n";
    bool sent = false;
    QList<ClientConnection *> clients;
    clients = Server::instance(tao)->clientConnections();
    foreach (ClientConnection *client, clients)
    {
        if (client->currentHookId() == id)
        {
            // If we were to call client->sendText(+msg) synchronously, we may
            // end up with mangled text because the ClientConnection thread
            // might be currently writing to the socket.
            QMetaObject::invokeMethod(client, "sendText", Qt::QueuedConnection,
                                      Q_ARG(QString, +msg));
            sent = true;
        }
    }

    return sent ? XL::xl_true : XL::xl_false;
}



// ============================================================================
//
//    Module glue
//
// ============================================================================

int module_init(const Tao::ModuleApi *api, const Tao::ModuleInfo *)
// ----------------------------------------------------------------------------
//   Initialize the Tao module
// ----------------------------------------------------------------------------
{
    XL_INIT_TRACES();
    tao = api;
    return 0;
}


int module_exit()
// ----------------------------------------------------------------------------
//   Uninitialize the Tao module
// ----------------------------------------------------------------------------
{
    Server::destroy();
    HookManager::destroy();
    return 0;
}



// Define traces for this modules
XL_DEFINE_TRACES

