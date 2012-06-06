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


#include <QObject>
#include <QByteArray>
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

public slots:
    void                setCurrentHookId(int id);
    void                setExecOnce(bool once);

protected slots:
    void                onReadyRead();

protected:
    std::ostream &      debug();
    void                processCommand(QString cmd);
    void                processSetHook(QString cmd);
    void                processXlCommand(QString cmd);

    void                sendText(QString msg);
    void                sendGreetings();
    void                sendPrompt();
    void                sendHelp();

    void                setHookCode(QString cmd);

private:
    QAbstractSocket *   socket;
    int                 currentHook;
    QByteArray          pending;
};

#endif // CLIENT_CONNECTION_H
