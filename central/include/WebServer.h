//
// Created by p.szwed
//

#ifndef ROSA_CENTRAL_WEBSERVER_H
#define ROSA_CENTRAL_WEBSERVER_H


#include "SessionList.h"
#include "SensorList.h"
#include "server_http.hpp"
#include "status_code.hpp"
#include "utility.hpp"


using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

class WebServer
{
    SessionList* sessionList;
    SensorList* sensorList;
    HttpServer* httpServer;
public:

    WebServer(SessionList* sessionList, SensorList* sensorList, HttpServer* server);


    static void validateSession(std::shared_ptr<HttpServer::Request> request, SessionList* sessionList);
    static void log(std::string message);
    static std::string getTimestamp();
};

#endif //ROSA_CENTRAL_WEBSERVER_H
