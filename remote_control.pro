# ******************************************************************************
#  remote_control.pro                                               Tao project
# ******************************************************************************
# File Description:
# Qt build file for the RemoteControl module
# ******************************************************************************
# This software is property of Taodyne SAS - Confidential
# Ce logiciel est la propriété de Taodyne SAS - Confidentiel
# (C) 2012 Jerome Forissier <jerome@taodyne.com>
# (C) 2012 Taodyne SAS
# ******************************************************************************

MODINSTDIR = remote_control

include(../modules.pri)

MODVERSION = 1.021
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
