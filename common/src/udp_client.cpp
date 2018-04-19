#include "udp_client.h"
#include "error_handler.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)
#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)

common::UDPClient::UDPClient(const char *host, const char *port)
    : socket(0), serverAddrinfo(nullptr) // port 0 - bind socket to an ephemeral port
{
    addrinfo hints;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if(getaddrinfo(host, port, &hints, &serverAddrinfo) != 0)
        FATAL_ERROR("getaddrinfo");
    if(serverAddrinfo->ai_next) //TODO - more than 1 result in the list - why does it happen?
    {
        char buffer[128];
        addrinfo *tmp = serverAddrinfo;
        std::cout << "warning - many addrinfo results:\n";

        for(int i=1; tmp != nullptr; tmp = tmp->ai_next, ++i)
        {
            sockaddr *sa = tmp->ai_addr;
            uint16_t port = (sa->sa_family == AF_INET) ?
                        ((struct sockaddr_in*)sa)->sin_port : (((struct sockaddr_in6*)sa)->sin6_port);
            inet_ntop(tmp->ai_family, tmp->ai_addr, buffer, sizeof buffer);
            std::cout << buffer << ":" << ntohs(port) << ", protocol: " << tmp->ai_protocol << "\n";
        }
    }
}

common::UDPClient::~UDPClient()
{
    if(serverAddrinfo)
        freeaddrinfo(serverAddrinfo);
}

const addrinfo *common::UDPClient::getServerAddrinfo()
{
   return serverAddrinfo;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
    sockaddr addr;
    socklen_t addrlen;
    int retval = recvfrom(socket.fd, buffer, size, 0, &addr, &addrlen);
    //TODO - chceck if addr is the same as the address we sent datagram to
    if(retval < 0)
        ERROR("no data received");
    return retval;
}

int common::UDPClient::send(const char *data, size_t size)
{
    int retval = sendto(socket.fd, data, size, 0, serverAddrinfo->ai_addr, serverAddrinfo->ai_addrlen);
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
#undef FATAL_ERROR
