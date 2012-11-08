#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H
// ****************************************************************************
//  client_connection.h                                            Tao project
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
