#ifndef SERVER_H
#define SERVER_H
// ****************************************************************************
//  server.h                                                       Tao project
// ****************************************************************************
//
//   File Description:
//
//    TCP server accepts commands from remote clients. Commands are handed
//    to Hook objects for later execution in the Tao document.
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


#include "tao/module_api.h"
#include <QList>
#include <QSet>
#include <QTcpServer>
#include <iostream>


class ClientConnection;

class Server : public QTcpServer
// ----------------------------------------------------------------------------
//    Command server is a singleton
// ----------------------------------------------------------------------------
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    typedef qintptr handle_t;
#else
    typedef int handle_t;
#endif

public:
    static Server *     instance(const Tao::ModuleApi *tao, int port = 6560);
    static bool         started() { return inst; }
    static void         destroy() { if (inst) { delete inst; inst = NULL; } }

public:
    QList<ClientConnection *> clientConnections();

protected:
    Server(const Tao::ModuleApi *tao, int port);
    virtual ~Server();

protected:
    virtual void        incomingConnection(handle_t handle);

protected:
    std::ostream &      debug();

private slots:
    void                onDisconnected();

private:
    typedef QSet<ClientConnection *> client_set;
    const Tao::ModuleApi * tao;
    bool                licensed;
    client_set          clients;

private:
    static Server *     inst;
};

#endif // SERVER_H
