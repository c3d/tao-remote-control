// ****************************************************************************
//  client_connection.cpp                                          Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************


#include "client_connection.h"
#include "hook_manager.h"
#include "hook.h"
#include "base.h"
#include <QHostAddress>
#include <QAbstractSocket>
#include <QStringList>
#include <QRegExp>


inline std::string operator +(QString s)
// ----------------------------------------------------------------------------
//   Convert QString to std::string
// ----------------------------------------------------------------------------
{
    return std::string(s.toUtf8().constData());
}


ClientConnection::ClientConnection(QAbstractSocket *socket)
// ----------------------------------------------------------------------------
//   Creation
// ----------------------------------------------------------------------------
    : socket(socket), currentHook(0)
{
    IFTRACE(remotecontrol)
        debug() << "New connection\n";

    Q_ASSERT(socket);
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));

    sendGreetings();
}


ClientConnection::~ClientConnection()
// ----------------------------------------------------------------------------
//   Delete connection
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Deleting\n";
    delete socket;
}


void ClientConnection::setCurrentHookId(int id)
// ----------------------------------------------------------------------------
//   Direct subsequent commands to hook id
// ----------------------------------------------------------------------------
{
    currentHook = id;
}


void ClientConnection::setExecOnce(bool once)
// ----------------------------------------------------------------------------
//   Subsequent commands to be executed once or on each refresh
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Set execOnce: " << once << "\n";
    HookManager::instance()->hook(currentHook)->execOnce = once;
}


void ClientConnection::onReadyRead()
// ----------------------------------------------------------------------------
//   Read incoming data
// ----------------------------------------------------------------------------
{
    QByteArray line;
    forever
    {
        line = socket->readLine();
        if (line.isEmpty())
            break;
        pending.append(line);
        if (!pending.endsWith('\n') && !pending.endsWith('\r'))
            continue;
        QString command = QString::fromUtf8(pending.constData()).trimmed();
        pending.clear();
        if (!command.isEmpty())
            processCommand(command);
        sendPrompt();
    }
}


void ClientConnection::processCommand(QString cmd)
// ----------------------------------------------------------------------------
//   Decode command and dispatch it to appropriate hook
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Command received: [" << +cmd << "]\n";

    QRegExp setHook("^#\\d+$"),
            xlCmd("^xl");

    if (cmd == "help")
        sendHelp();
    else if (cmd == "exit" || cmd == "quit" || cmd == "q")
        processExit();
    else if (setHook.indexIn(cmd) != -1)
        processSetHook(cmd);
    else if (xlCmd.indexIn(cmd) != -1)
        processXlCommand(cmd);
    else
        sendText("Unknown command or syntax error.\n");
}


void ClientConnection::processSetHook(QString cmd)
// ----------------------------------------------------------------------------
//   Change current target hook
// ----------------------------------------------------------------------------
{
    currentHook = cmd.mid(1).toInt();
    IFTRACE(remotecontrol)
        debug() << "Current hook changed\n";
}


void ClientConnection::processXlCommand(QString cmd)
// ----------------------------------------------------------------------------
//   Execute raw XL code
// ----------------------------------------------------------------------------
{
    QString xl;
    QRegExp xlCmd("xl(.*)");
    if (xlCmd.indexIn(cmd) > -1)
        xl = xlCmd.cap(1);
    xl = xl.trimmed();
    if (!xl.isEmpty())
        setHookCode(xl);
}


void ClientConnection::processExit()
// ----------------------------------------------------------------------------
//   Disconnect client session
// ----------------------------------------------------------------------------
{
    sendText("Bye.\n");
    socket->close();
}


void ClientConnection::sendText(QString msg)
// ----------------------------------------------------------------------------
//   Send text to client
// ----------------------------------------------------------------------------
{
    msg.replace(QChar('\n'), "\r\n");
    QByteArray ba(msg.toUtf8().constData());
    socket->write(ba);
}


void ClientConnection::sendPrompt()
// ----------------------------------------------------------------------------
//   Send help text to client
// ----------------------------------------------------------------------------
{
    QString prompt = QString("#%1> ").arg(currentHook);
    sendText(prompt);
}


void ClientConnection::sendGreetings()
// ----------------------------------------------------------------------------
//   Send text to newly connected client
// ----------------------------------------------------------------------------
{
    QString msg("Tao Presentations Remote Control Server ready.\n");
    sendText(msg);
    sendPrompt();
}


void ClientConnection::sendHelp()
// ----------------------------------------------------------------------------
//   Send help text to client
// ----------------------------------------------------------------------------
{
    sendText("Sorry, no help is available.\n");
}


void ClientConnection::setHookCode(QString cmd)
// ----------------------------------------------------------------------------
//   Set the XL code to be executed by the hook
// ----------------------------------------------------------------------------
{
    HookManager::instance()->hook(currentHook)->setCommand(+cmd);
}


std::ostream & ClientConnection::debug()
// ----------------------------------------------------------------------------
//   Convenience method to log with a common prefix
// ----------------------------------------------------------------------------
{
    std::cerr << "[ClientConnection " << (void*)this;
    if (socket && socket->state() == QAbstractSocket::ConnectedState)
        std::cerr << " " << +socket->peerAddress().toString() << ":"
                  << socket->peerPort();
    std::cerr << "] #" << currentHook << " ";
    return std::cerr;
}
