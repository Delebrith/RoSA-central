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
    : socket(0), serverAddressInfo(host, port, SOCK_DGRAM), serverAddress(serverAddressInfo) // port 0 - bind socket to an ephemeral port
{
    std::cout << "getaddrinfo result:\n";
    serverAddressInfo.forEach([](addrinfo *ai) { Address(ai).print(std::cout); });
}

const common::AddressInfo &common::UDPClient::getServerAddrinfo() const
{
   return serverAddressInfo;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
//    sockaddr addr;
//    socklen_t addrlen;
    int retval = recvfrom(socket.fd, buffer, size, 0, 0, 0);
    //TODO - chceck if addr is the same as the address we sent datagram to
    if(retval < 0)
        ERROR("no data received");
    return retval;
}

int common::UDPClient::send(const char *data, size_t size)
{
    int retval = sendto(socket.fd, data, size, 0, serverAddress.getAddress(), serverAddress.getAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
#undef FATAL_ERROR
