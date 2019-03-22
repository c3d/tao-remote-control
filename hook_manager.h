#ifndef HOOK_MANAGER_H
#define HOOK_MANAGER_H
// *****************************************************************************
// hook_manager.h                                                  Tao3D project
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
// (C) 2012,2014,2019, Christophe de Dinechin <christophe@dinechin.org>
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


#include "base.h"
#include <QObject>
#include <QMap>
#include <QList>
#include <iostream>

class Hook;

class HookManager
// ----------------------------------------------------------------------------
//   Singleton to manage a collection of Hooks
// ----------------------------------------------------------------------------
{
public:
    HookManager();
    virtual ~HookManager();

public:
    static
    HookManager *       instance();
    static
    void                destroy() { if (inst) { delete inst; inst = NULL; } }

public:
    Hook *              hook(int id);
    void                remove(int id);
    QList<int>          ids();

protected:
    typedef QMap<int, Hook *> hook_map;
    hook_map            hooks;
    bool                licensed;

protected:
    static
    HookManager *       inst;
};


#endif // HOOK_H
