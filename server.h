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
    static Server *     instance();

protected:
    Server();
    virtual ~Server() {}

protected:
    std::ostream &      debug();

private slots:
    void                onNewConnection();
    void                onDisconnected();

private:
    typedef std::map<QTcpSocket *, ClientConnection *> client_map;
    client_map          clients;

private:
    static Server *     inst;
};

#endif // SERVER_H
