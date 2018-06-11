//
// Created by p.szwed
//

#ifndef ROSA_CENTRAL_WEBSERVER_H
#define ROSA_CENTRAL_WEBSERVER_H


#include "SessionList.h"
#include "Communicator.h"
#include "server_http.hpp"
#include "status_code.hpp"
#include "utility.hpp"


using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

class WebServer
{
public:

    WebServer(SessionList* sessionList, Communicator* communicator, HttpServer* server);
    static void validateSession(std::shared_ptr<HttpServer::Request> request, SessionList* sessionList);
};

#endif //ROSA_CENTRAL_WEBSERVER_H
