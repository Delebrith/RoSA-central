#include "udp_socket.h"
#include "error_handler.h"
#include <unistd.h>
#include <cstring>

#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)
#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)

common::UDPsocket::UDPsocket(uint16_t port)
    : fd(-1), ownAddress(port)
{
    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
        FATAL_ERROR("creating socket");
// The code below prodeuces an 'invalid argument' error, IDK why:
//    int disable = 0;
//    if(setsockopt(fd, SOL_SOCKET, IPV6_V6ONLY, &disable, sizeof(int)) < 0)
//        FATAL_ERROR("setsockopt(IPV6ONLY) disable");
// Disabling IPV6ONLY should allow dual IP stack (usage of both IPv6 and IPv4 on one socket).
    int enable = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        FATAL_ERROR("setsockopt(SO_REUSEADDR) enable");
    if(bind(fd, ownAddress.getAddress(), ownAddress.getAddressLength()) < 0)
        FATAL_ERROR("binding socket");
}

int common::UDPsocket::getFd() const
{
    return fd;
}

void common::UDPsocket::setSendTimeout(unsigned milliseconds)
{
    timeval timeout = { milliseconds/1000, 1000*(milliseconds%1000) };
    if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
        ERROR("setsockopt(SO_SNDTIMEO");
}

void common::UDPsocket::setReceiveTimeout(unsigned milliseconds)
{
    timeval timeout = { milliseconds/1000, 1000*(milliseconds%1000) };
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
        ERROR("setsockopt(SO_RCVTIMEO)");
}

common::UDPsocket::~UDPsocket()
{
    if(fd > 0)
        close(fd);
}
