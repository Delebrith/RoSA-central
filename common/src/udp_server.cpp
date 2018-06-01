#include "udp_server.h"
#include "exception.h"
#include <unistd.h>
#include <iostream>
#ifndef NDEBUG
#include <iostream>
#endif

common::UDPServer::UDPServer(uint16_t port)
    : socket(port), clientAddressIsCorrect(false)
{}

common::UDPsocket &common::UDPServer::getSocket()
{
    return socket;
}

const common::Address &common::UDPServer::getClientAddress() const
{
    return clientAddress;
}

int common::UDPServer::receive(char *buffer, size_t size)
{
    int retval = recvfrom(socket.getFd(), buffer, size, 0, clientAddress.getAddress(), clientAddress.getAddressLengthPointer());
    if(retval < 0)
    {
        clientAddressIsCorrect = false;
        if(errno != EAGAIN && errno != EWOULDBLOCK) // non-standard error (not timeout / resource unavailability)
            ExceptionInfo::warning("receive failed with errno: " + std::string(strerror(errno)));
        return retval;
    }
#ifndef NDEBUG
    std::cout << "received from: ";
    clientAddress.print(std::cerr);
#endif
    clientAddressIsCorrect = true;
    return retval;
}

int common::UDPServer::send(const char *data, size_t size)
{
    if(!clientAddressIsCorrect)
    {
        throw Exception("attempt to send message from server before calling receive - client address is unknown");
        return false;
    }
    clientAddress.incrementPort(); // server sends answer to port from which message was received + 1
    clientAddressIsCorrect = false; // server works in simple model receive->send->receive..., so no multiple sends will be allowed
    int retval = sendto(socket.getFd(), data, size, 0, clientAddress.getAddress(), clientAddress.getAddressLength());
    if(retval < 0) // sending UDP packet should generally not fail, even if packet will not be received
        ExceptionInfo::warning("send failed with errno: " + std::string(strerror(errno)));
    return retval;
}
