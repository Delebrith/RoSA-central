#include "udp_server.h"
#include "error_handler.h"
#include <unistd.h>
#include <iostream>

#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)

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
        ERROR("no data received");
        clientAddressIsCorrect = false;
        return retval;
    }
    std::cout << "received from: ";
    clientAddress.print(std::cout);
    clientAddressIsCorrect = true;
    return retval;
}

int common::UDPServer::send(const char *data, size_t size)
{
    if(!clientAddressIsCorrect)
    {
        ERROR("unknown client");
        return false;
    }
    int retval = sendto(socket.getFd(), data, size, 0, clientAddress.getAddress(), clientAddress.getAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
