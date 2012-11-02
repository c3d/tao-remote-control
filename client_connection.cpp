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
#include "tao/tao_utf8.h"
#include <QHostAddress>
#include <QAbstractSocket>
#include <QStringList>
#include <QRegExp>
#include <QTcpSocket>


using namespace Tao;


ClientConnection::ClientConnection(int socketDescriptor)
// ----------------------------------------------------------------------------
//   Creation
// ----------------------------------------------------------------------------
    : socket(NULL), socketDescriptor(socketDescriptor), currentHook(0),
      prompt(true)
{
    IFTRACE(remotecontrol)
        debug() << "New connection\n";

    // Pre-defined macros. Example: command @quit will map to XL code "exit 0".
    macros["quit"]    = "xl exit 0";
    macros["q"]       = "@quit";
    macros["nextp"]   = "xl! if page_number<page_count then "
                        "goto_page page_name (page_number+1)";
    macros["prevp"]   = "xl! if page_number>1 then "
                        "goto_page page_name (page_number-1)";
    macros["play"]    = "xl! enable_animations true";
    macros["pause"]   = "xl! enable_animations false";
    macros["blank"]   = "xl! blank_screen true";
    macros["noblank"] = "xl! blank_screen false";
    macros["tblank"]  = "xl! toggle_blank_screen";
    macros["fs"]      = "xl! full_screen true";
    macros["nofs"]    = "xl! full_screen false";
    macros["tfs"]     = "xl! toggle_full_screen";
}


ClientConnection::~ClientConnection()
// ----------------------------------------------------------------------------
//   Delete connection
// ----------------------------------------------------------------------------
{
    Q_ASSERT(QThread::currentThread() != thread());

    IFTRACE(remotecontrol)
        debug() << "Stopping thread\n";
    quit();
    wait();
    IFTRACE(remotecontrol)
        debug() << "Deleting socket\n";
    QTcpSocket *s = socket;
    socket = NULL;
    s->deleteLater(); // delete in this object's thread
}


void ClientConnection::onDisconnected()
// ----------------------------------------------------------------------------
//   Tell thread to quit and forward signal
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Disconnected\n";
    if (socket)
    {
        // Don't notify server if it is the server that is deleting us
        emit disconnected();
    }
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
    IFTRACE(remotecontrolcmd)
        debug() << "Command [" << +cmd << "]\n";

    if (cmd == "help" || cmd == "?")
        sendHelp();
    else if (cmd == "exit" || cmd == "quit" || cmd == "q")
        processExit();
    else if (cmd == "@")
        listMacros();
    else if (cmd == "#")
        listHooks();
    else if (QRegExp("^#\\d+$").indexIn(cmd) != -1)
        processSetHook(cmd);
    else if (cmd.startsWith('@'))
        processMacro(cmd);
    else if (QRegExp("^set\\s").indexIn(cmd) != -1)
        processSetMacro(cmd);
    else if (QRegExp("^unset\\s").indexIn(cmd) != -1)
        processUnsetMacro(cmd);
    else if (cmd == "xl" || cmd == "xl!")
        runXl("");
    else if (QRegExp("^xl\\s").indexIn(cmd) != -1)
        processXlCommand(cmd);
    else if (QRegExp("^xl!\\s").indexIn(cmd) != -1)
        processXlCommand(cmd, true);
    else if (cmd.startsWith("prompt"))
        processPromptCommand(cmd);
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


void ClientConnection::processSetMacro(QString cmd)
// ----------------------------------------------------------------------------
//   Define macro
// ----------------------------------------------------------------------------
{
    QRegExp re("set\\s+(\\w+)\\s+(.*)");
    if (re.indexIn(cmd) > -1)
    {
        QString name = re.cap(1);
        QString val = re.cap(2).trimmed();
        macros[name] = val;
        IFTRACE(remotecontrol)
            debug() << "Macro defined: @" << +name << " = " << +val << "\n";
    }
}


void ClientConnection::processUnsetMacro(QString cmd)
// ----------------------------------------------------------------------------
//   Un-define macro
// ----------------------------------------------------------------------------
{
    QRegExp re("unset\\s+(\\w+)");
    if (re.indexIn(cmd) > -1)
    {
        QString name = re.cap(1);
        if (macros.remove(name) != 0)
        {
            IFTRACE(remotecontrol)
                debug() << "Macro @" << +name << " unset\n";
        }
    }
}

void ClientConnection::processMacro(QString cmd)
// ----------------------------------------------------------------------------
//   Run macro: @name
// ----------------------------------------------------------------------------
{
    runMacro(cmd.mid(1));
}


void ClientConnection::processXlCommand(QString cmd, bool once)
// ----------------------------------------------------------------------------
//   Execute raw XL code
// ----------------------------------------------------------------------------
{
    QString xl;
    QRegExp xlCmd = once ? QRegExp("xl!(.*)") : QRegExp("xl(.*)");
    if (xlCmd.indexIn(cmd) > -1)
        xl = xlCmd.cap(1);
    xl = xl.trimmed();
    if (!xl.isEmpty())
        runXl(xl, once);
}


void ClientConnection::processPromptCommand(QString cmd)
// ----------------------------------------------------------------------------
//   Disable, enable or toggle the prompt (#0>)
// ----------------------------------------------------------------------------
{
    QString parm;
    QRegExp re = QRegExp("prompt(.*)");
    if (re.indexIn(cmd) > -1)
        parm = re.cap(1).trimmed();
    if (parm == "")
        prompt = !prompt;
    else if (parm == "on")
        prompt = true;
    else if (parm == "off")
        prompt = false;
    else
        sendText("Invalid parameter: " + parm + "\n");
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
    // Don't call from a thread that is not the connection thread, or text
    // could possibly be mangled
    Q_ASSERT(QThread::currentThread() == thread());

    msg.replace(QChar('\n'), "\r\n");
    QByteArray ba(msg.toUtf8().constData());
    socket->write(ba);
}


void ClientConnection::sendPrompt()
// ----------------------------------------------------------------------------
//   Send prompt to client
// ----------------------------------------------------------------------------
{
    if (prompt)
    {
        QString prompt = QString("#%1> ").arg(currentHook);
        sendText(prompt);
    }
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
    QString help;

#define _H(x) help += QString(x)
    _H("Commands:\n");
    _H("  #<integer>\n");
    _H("      Select the current hook (default is number 0).\n");
    _H("      Example:  #1\n");
    _H("  #\n");
    _H("      Show all hooks.\n");
    _H("      Hooks are created when Tao executes remote_control_hook or\n");
    _H("      when some code is assigned from a client connection (xl, xl!\n");
    _H("      commands).\n");
    _H("  @<name>\n");
    _H("      Execute a macro.\n");
    _H("      Example:  @quit\n");
    _H("  @\n");
    _H("      Show all macros defined for the current connection.\n");
    _H("  prompt [on|off]\n");
    _H("      Enable or disable the display of the command prompt.\n");
    _H("      With no parameter, the command toggles the current state.\n");
    _H("      Example:  prompt off\n");
    _H("  set <name> <value>\n");
    _H("      Define/replace macro.\n");
    _H("      Example:  set quit xl exit 0\n");
    _H("  unset <name>\n");
    _H("      Undefine a macro.\n");
    _H("      Example:  unset quit\n");
    _H("  xl <code>\n");
    _H("      Execute XL code. Any occurrence of remote_control_hook for\n");
    _H("      the current hook becomes equivalent to the specified code.\n");
    _H("      If <code> is empty, hook evaluates to false.\n");
    _H("      Example:  xl locally {color \"red\"; circle 0, 0, 100}\n");
    _H("  xl! <code>\n");
    _H("      Execute XL code once. Any occurence of remote_control_hook\n");
    _H("      for the current hook will execute the specified code at most\n");
    _H("      once.\n");
    _H("      If <code> is empty, hook evaluates to false.\n");
    _H("      Example:  xl! if page_number>1 then goto_page page_name (page_number-1)\n");
    _H("  help, ?\n");
    _H("      Show this help.\n");
    _H("  quit, q\n");
    _H("      Disconnect.\n");
#undef _H

    sendText(help);
}


void ClientConnection::listMacros()
// ----------------------------------------------------------------------------
//   Dump all macros and their definition
// ----------------------------------------------------------------------------
{
    QList<QString> names(macros.keys());
    foreach (QString name, names)
    {
        QString msg = QString(" @%1 = %2\n").arg(name).arg(macros[name]);
        sendText(msg);
    }
}


void ClientConnection::listHooks()
// ----------------------------------------------------------------------------
//   Show all hooks
// ----------------------------------------------------------------------------
{
    HookManager * mgr = HookManager::instance();
    QList<int> ids = mgr->ids();
    QString msg;
    foreach (int id, ids)
    {
        Hook * hook = mgr->hook(id);
        Q_ASSERT(hook);
        msg = QString(" #%1 '%2'\n").arg(id).arg(+hook->command());
        sendText(msg);
    }
}


void ClientConnection::runXl(QString cmd, bool once)
// ----------------------------------------------------------------------------
//   Set the XL code to be executed by the hook
// ----------------------------------------------------------------------------
{
    Hook *hook = HookManager::instance()->hook(currentHook);
    // Call setCommand from the main thread
    QMetaObject::invokeMethod(hook, "setCommand", Qt::QueuedConnection,
                              Q_ARG(QString, cmd), Q_ARG(bool, once));
}


void ClientConnection::runMacro(QString name)
// ----------------------------------------------------------------------------
//   Expand macro and run it
// ----------------------------------------------------------------------------
{
    if (!macros.contains(name))
    {
        QString err = QString("%1: undefined macro.\n").arg(name);
        sendText(err);
        return;
    }
    processCommand(macros[name]);
}


void ClientConnection::disconnect(QString msg)
// ----------------------------------------------------------------------------
//   Disconnect client
// ----------------------------------------------------------------------------
{
    sendText(msg);
    socket->close();
}


void ClientConnection::run()
// ----------------------------------------------------------------------------
//   Entry point for the connection thread
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Thread starting\n";

    socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        IFTRACE(remotecontrol)
            debug() << "Socket error: " << socket->error() << "\n";
        delete socket;
        return;
    }
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    sendGreetings();

    exec();
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
