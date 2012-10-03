// ****************************************************************************
//  remote_control.cpp                                             Tao project
// ****************************************************************************
// 
//   File Description:
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
// ****************************************************************************
//  (C) 2012 Jérôme Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************

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

