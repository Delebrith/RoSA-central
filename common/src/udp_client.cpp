#include "udp_client.h"
#include "error_handler.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef DEBUG
#include <iostream>
#endif

#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)
#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)

common::UDPClient::UDPClient(const char *host, const char *port)
    : socket(0), serverAddressInfo(host, port, SOCK_DGRAM), serverAddress(serverAddressInfo) // port 0 - bind socket to an ephemeral port
{
#ifdef DEBUG
    std::cout << "getaddrinfo result:\n";
    serverAddressInfo.forEach([](addrinfo *ai) { Address(ai).print(std::cout); });
#endif
}

common::UDPsocket &common::UDPClient::getSocket()
{
    return socket;
}

const common::AddressInfo &common::UDPClient::getServerAddrinfo() const
{
   return serverAddressInfo;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
    Address client_address;
    int retval = recvfrom(socket.getFd(), buffer, size, 0, client_address.getAddress(), client_address.getAddressLengthPointer());
    //TODO - chceck if addr is the same as the address we sent datagram to
    if(retval < 0)
        ERROR("no data received");
    else
    {
#ifdef DEBUG
        std::cout << "client received answer from:\n";
        client_address.print(std::cout);
#endif
    }
    return retval;
}

int common::UDPClient::send(const char *data, size_t size)
{
    int retval = sendto(socket.getFd(), data, size, 0, serverAddress.getAddress(), serverAddress.getAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
#undef FATAL_ERROR
