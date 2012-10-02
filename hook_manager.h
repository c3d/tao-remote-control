#ifndef HOOK_MANAGER_H
#define HOOK_MANAGER_H
// ****************************************************************************
//  hook_manager.h                                                 Tao project
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


#include "base.h"
#include <QObject>
#include <QMap>
#include <QList>
#include <iostream>

class Hook;

class HookManager : public QObject
// ----------------------------------------------------------------------------
//   Singleton to manage a collection of Hooks
// ----------------------------------------------------------------------------
{
    Q_OBJECT

public:
    HookManager();
    virtual ~HookManager() {}

public:
    static
    HookManager *       instance();

public:
    Hook *              hook(int id);
    void                remove(int id);
    QList<int>          ids();

protected:
    std::ostream &      debug();

protected:
    typedef QMap<int, Hook *> hook_map;
    hook_map            hooks;
    bool                licensed;

protected:
    static
    HookManager *       inst;
};


#endif // HOOK_H
