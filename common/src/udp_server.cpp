#include "udp_server.h"
#include "error_handler.h"
#include <unistd.h>

#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)

common::UDPServer::UDPServer(uint16_t port)
    : socket(port), clientAddressIsCorrect(false)
{}

const sockaddr *common::UDPServer::getClientAddress()
{
    return &clientAddress;
}

socklen_t common::UDPServer::getClientAddressLength()
{
    return clientAddressLength;
}

int common::UDPServer::receive(char *buffer, size_t size)
{
    int retval = recvfrom(socket.fd, buffer, size, 0, &clientAddress, &clientAddressLength);
    if(retval < 0)
    {
        ERROR("no data received");
        clientAddressIsCorrect = false;
        return retval;
    }
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
    int retval = sendto(socket.fd, data, size, 0, getClientAddress(), getClientAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
