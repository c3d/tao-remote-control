#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H
// *****************************************************************************
// client_connection.h                                             Tao3D project
// *****************************************************************************
//
// File description:
//
//    Re-assemble commands coming from a remote client as a byte stream.
//    Hand over complete commands to Hook objects for later execution in the
//    Tao document.
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


#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QString>
#include <QThread>
#include <iostream>

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class ClientConnection : public QThread
// ----------------------------------------------------------------------------
//    Re-assemble commands and dispatch them to Hook objects
// ----------------------------------------------------------------------------
{
    Q_OBJECT

public:
    ClientConnection(int socketDescriptor);
    virtual ~ClientConnection();

public:
    int                 currentHookId() { return currentHook; }

public slots:
    void                sendText(QString msg);
    void                disconnect(QString msg);

signals:
    void                disconnected();

protected slots:
    void                onDisconnected();
    void                onReadyRead();

protected:
    virtual void        run();

    std::ostream &      debug();
    void                processCommand(QString cmd);
    void                processSetHook(QString cmd);
    void                processSetMacro(QString cmd);
    void                processUnsetMacro(QString cmd);
    void                processMacro(QString cmd);
    void                processXlCommand(QString cmd, bool once = false);
    void                processExit();
    void                processPromptCommand(QString cmd);

    void                sendGreetings();
    void                sendPrompt();
    void                sendHelp();
    void                listMacros();
    void                listHooks();

    void                runXl(QString cmd, bool once = false);
    void                runMacro(QString name);

private:
    typedef QMap<QString, QString> macro_map;

    QTcpSocket *        socket;
    int                 socketDescriptor;
    int                 currentHook;
    QByteArray          pending;
    macro_map           macros;
    bool                prompt;
};

#endif // CLIENT_CONNECTION_H
