#ifndef HOOK_H
#define HOOK_H
// *****************************************************************************
// hook.h                                                          Tao3D project
// *****************************************************************************
//
// File description:
//
//    Execute commands sent from remote clients.
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

#include "context.h"
#include "tree.h"
#include <QMutex>
#include <QObject>
#include <QList>
#include <iostream>


class Hook : public QObject
// ----------------------------------------------------------------------------
//   Execute commands in a Tao document
// ----------------------------------------------------------------------------
{
    Q_OBJECT
public:
    struct Command
    {
        Command(text cmd, bool once) : cmd(cmd), once(once) {}

        text cmd;
        bool once;
    };

public:
    Hook(int id);
    virtual ~Hook() {}

public:
    // Primitives
    static
    XL::Tree_p          eval(XL::Context *context, XL::Tree_p self, text code);
    static
    XL::Tree_p          evalOnce(XL::Context *context, XL::Tree_p self,
                                 text code);

public:
    XL::Tree_p          exec(XL::Context *context, XL::Tree_p self);

    Command             commandPeek();
    Command             command();

public slots:
    void                setCommand(QString cmd, bool once = false);

protected:
    static
    XL::Tree_p          evalInternal(XL::Context *context, XL::Tree_p self,
                                     text code, bool once = false);

protected:
    std::ostream &      debug();

private:
    QMutex              mutex;
    int                 id;
    text                cmd;
    QList<text>         onceCommands;
    int                 onceCounter;
public:
    int                 refreshEvent;
};


#endif // HOOK_H
