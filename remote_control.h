#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H
// *****************************************************************************
// remote_control.h                                                Tao3D project
// *****************************************************************************
//
// File description:
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
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2014,2019, Christophe de Dinechin <christophe@dinechin.org>
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

#include "tree.h"
#include "context.h"

// Primitives
XL::Tree_p remoteControlHook(XL::Context *context, XL::Tree_p self, int id,
                             int port);
XL::Tree_p remoteControlWriteln(XL::Tree_p self, int id, text msg,
                                bool once = false);

#endif // REMOTE_CONTROL_H
