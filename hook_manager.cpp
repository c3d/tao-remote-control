// *****************************************************************************
// hook_manager.cpp                                                Tao3D project
// *****************************************************************************
//
// File description:
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
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012,2014-2015,2019, Christophe de Dinechin <christophe@dinechin.org>
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
    XL_ASSERT(hooks.contains(id));
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
