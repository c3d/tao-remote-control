#ifndef HOOK_H 
#define HOOK_H 
// ****************************************************************************
//  hook.h                                                         Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is licensed under the GNU General Public License v3.
// See file COPYING for details.
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************

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
