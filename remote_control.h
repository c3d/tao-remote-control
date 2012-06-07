#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H
// ****************************************************************************
//  remote_control.h                                               Tao project
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
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************

#include "tree.h"
#include "context.h"

// Primitives
XL::Tree_p remoteControlHook(XL::Context *context, XL::Tree_p self, int id,
                             int port);
XL::Tree_p remoteControlWriteln(int id, text msg);

#endif // REMOTE_CONTROL_H
