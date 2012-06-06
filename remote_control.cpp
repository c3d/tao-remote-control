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
#include "tao/module_api.h"

using namespace Tao;


// Pointer to Tao functions
const Tao::ModuleApi *tao = NULL;


//static bool hasLicence()
//// ----------------------------------------------------------------------------
////   Check if we have a valid licence for this feature
//// ----------------------------------------------------------------------------
//{
//    static bool result = tao->checkImpressOrLicense("RemoteControl 1.0");
//    return result;
//}



XL::Tree_p remoteControlHook(XL::Context *context, XL::Tree_p self,
                             int id, int /*port*/)
// ----------------------------------------------------------------------------
//    Insertion point for arbitrary commands from remote client
// ----------------------------------------------------------------------------
{
    // First call instantiates server
    (void)Server::instance();

    Hook * hook = HookManager::instance()->hook(id);
    hook->exec(context, self);
    tao->refreshOn(hook->refreshEvent, -1.0);

    return XL::xl_false;
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
    return 0;
}



// Define traces for this modules
XL_DEFINE_TRACES

