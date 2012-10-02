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
#include <iostream>

QT_BEGIN_NAMESPACE
class QAbstractSocket;
QT_END_NAMESPACE

class ClientConnection : public QObject
// ----------------------------------------------------------------------------
//    Re-assemble commands and dispatch them to Hook objects
// ----------------------------------------------------------------------------
{
    Q_OBJECT

public:
    ClientConnection(QAbstractSocket *socket);
    virtual ~ClientConnection();

public:
    void                sendText(QString msg);
    int                 currentHookId() { return currentHook; }
    void                disconnect(QString msg);

public slots:
    void                setCurrentHookId(int id);

protected slots:
    void                onReadyRead();

protected:
    std::ostream &      debug();
    void                processCommand(QString cmd);
    void                processSetHook(QString cmd);
    void                processSetMacro(QString cmd);
    void                processUnsetMacro(QString cmd);
    void                processMacro(QString cmd);
    void                processXlCommand(QString cmd, bool once = false);
    void                processExit();

    void                sendGreetings();
    void                sendPrompt();
    void                sendHelp();
    void                listMacros();
    void                listHooks();

    void                runXl(QString cmd, bool once = false);
    void                runMacro(QString name);

private:
    typedef QMap<QString, QString> macro_map;

    QAbstractSocket *   socket;
    int                 currentHook;
    QByteArray          pending;
    macro_map           macros;
};

#endif // CLIENT_CONNECTION_H
