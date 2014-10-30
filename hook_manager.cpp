// ****************************************************************************
//  hook_manager.cpp                                               Tao project
// ****************************************************************************
//
//   File Description:
//
//    Keep collection of Hook objects, dispatch commands for execution in Tao.
//
//
//
//
//
//
//
//
// ****************************************************************************
// This software is licensed under the GNU General Public License v3.
// See file COPYING for details.
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************


#include "hook.h"
#include "hook_manager.h"
#include "base.h"
#include "runtime.h"
#include <QtGlobal>
#include <QString>
#include <QCoreApplication>


HookManager * HookManager::inst = NULL;


HookManager::HookManager()
// ----------------------------------------------------------------------------
//    Constructor
// ----------------------------------------------------------------------------
{}


HookManager::~HookManager()
// ----------------------------------------------------------------------------
//    Delete hooks
// ----------------------------------------------------------------------------
{
    foreach (Hook *hook, hooks.values())
        delete hook;
}


void HookManager::remove(int id)
// ----------------------------------------------------------------------------
//    Remove hook from map
// ----------------------------------------------------------------------------
{
    Q_ASSERT(hooks.contains(id));
    hooks.remove(id);
}


QList<int> HookManager::ids()
// ----------------------------------------------------------------------------
//    Return all hook IDs in ascending order
// ----------------------------------------------------------------------------
{
    QList<int> ids(hooks.keys());
    qSort(ids);
    return ids;
}


HookManager * HookManager::instance()
// ----------------------------------------------------------------------------
//    Return unique HookManager instance
// ----------------------------------------------------------------------------
{
    if (!inst)
        inst = new HookManager;
    return inst;
}


Hook * HookManager::hook(int id)
// ----------------------------------------------------------------------------
//    Find hook or create it
// ----------------------------------------------------------------------------
{
    if (!hooks.count(id))
    {
        Hook *hook = new Hook(id);
        // Make sure the hook's event loop runs in the main thread (see
        // Hook::setCommand()).
        // This is naturally the case when we're called from the XL primitive
        // (remoteControlHook()), but not when the Hook instance is created
        // by a Connection object (which runs its own event loop in its own
        // thread).
        hook->moveToThread(qApp->thread());
        hooks[id] = hook;
    }
    return hooks[id];
}
