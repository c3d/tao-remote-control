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
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************


#include "hook.h"
#include "hook_manager.h"
#include "base.h"
#include "runtime.h"
#include <QtGlobal>
#include <QString>


HookManager * HookManager::inst = NULL;


inline std::string operator +(QString s)
// ----------------------------------------------------------------------------
//   Convert QString to std::string
// ----------------------------------------------------------------------------
{
    return std::string(s.toUtf8().constData());
}


HookManager::HookManager()
// ----------------------------------------------------------------------------
//    Constructor
// ----------------------------------------------------------------------------
{}


void HookManager::remove(int id)
// ----------------------------------------------------------------------------
//    Remove hook from map
// ----------------------------------------------------------------------------
{
    Q_ASSERT(hooks.count(id));
    hooks.erase(id);
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
        hooks[id] = new Hook(id);
    return hooks[id];
}


std::ostream & HookManager::debug()
// ----------------------------------------------------------------------------
//   Convenience method to log with a common prefix
// ----------------------------------------------------------------------------
{
    std::cerr << "[HookManager] ";
    return std::cerr;
}
