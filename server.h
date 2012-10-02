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
#include <QMap>
#include <QList>
#include <QTcpServer>
#include <iostream>


class ClientConnection;

class Server : public QTcpServer
// ----------------------------------------------------------------------------
//    Command server is a singleton
// ----------------------------------------------------------------------------
{
    Q_OBJECT

public:
    static Server *     instance(const Tao::ModuleApi *tao, int port = 6560);
    static bool         started() { return inst; }

public:
    QList<ClientConnection *> clientConnections();

public slots:
    void                      disconnectNoLicense();

protected:
    Server(const Tao::ModuleApi *tao, int port);
    virtual ~Server() {}

protected:
    std::ostream &      debug();

private slots:
    void                onNewConnection();
    void                onDisconnected();

private:
    typedef QMap<QTcpSocket *, ClientConnection *> client_map;
    const Tao::ModuleApi * tao;
    bool                licensed;
    client_map          clients;

private:
    static Server *     inst;
};

#endif // SERVER_H
