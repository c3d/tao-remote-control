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
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************

#include "context.h"
#include "tree.h"
#include <QObject>
#include <map>
#include <iostream>


class Hook : public QObject
// ----------------------------------------------------------------------------
//   Execute commands in a Tao document
// ----------------------------------------------------------------------------
{
    Q_OBJECT

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

    void                setCommand(text cmd);

protected:
    static
    XL::Tree_p          evalInternal(XL::Context *context, XL::Tree_p self,
                                     text code, bool once = false);

protected:
    std::ostream &      debug();

private:
    int                 id;
public:
    text                command;
    bool                execOnce;
    int                 refreshEvent;
};


#endif // HOOK_H
