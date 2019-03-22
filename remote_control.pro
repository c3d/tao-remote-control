# ******************************************************************************
# remote_control.pro                                               Tao3D project
# ******************************************************************************
#
# File description:
# Qt build file for the RemoteControl module
#
#
#
#
#
#
#
# ******************************************************************************
# This software is licensed under the GNU General Public License v3
# (C) 2012-2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
# (C) 2012-2013, Catherine Burvelle <catherine@taodyne.com>
# (C) 2012-2014,2019, Christophe de Dinechin <christophe@dinechin.org>
# (C) 2012-2013, Jérôme Forissier <jerome@taodyne.com>
# ******************************************************************************
# This file is part of Tao3D
#
# Tao3D is free software: you can r redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Tao3D is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Tao3D, in a file named COPYING.
# If not, see <https://www.gnu.org/licenses/>.
# ******************************************************************************

MODINSTDIR = remote_control

include(../modules.pri)

MODVERSION = 1.022
DEFINES += MODVERSION=\\\"$$MODVERSION\\\"

INCLUDEPATH += $${TAOTOPSRC}/tao/include/tao/
HEADERS = client_connection.h \
          hook.h \
          hook_manager.h \
          remote_control.h \
          server.h

SOURCES = client_connection.cpp \
          hook.cpp \
          hook_manager.cpp \
          remote_control.cpp \
          server.cpp

TBL_SOURCES  = remote_control.tbl

OTHER_FILES = remote_control.xl remote_control.tbl traces.tbl
QT          += core network

# Icon from http://www.iconfinder.com/icondetails/18728/32/console_dos_terminal_icon
# Author: Everaldo Coelho (Crystal project)
# License: LGPL
INSTALLS    += thismod_icon

QMAKE_SUBSTITUTES = doc/Doxyfile.in
DOXYFILE = doc/Doxyfile
DOXYLANG = en,fr
include(../modules_doc.pri)
