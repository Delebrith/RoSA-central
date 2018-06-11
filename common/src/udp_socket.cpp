//
// Created by T. Nowak
//

#include "udp_socket.h"
#include "exception.h"
#include <unistd.h>
#include <cstring>

common::UDPsocket::UDPsocket(uint16_t port)
    : fd(-1), ownAddress(port)
{
    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
        throw Exception("creating socket failed with errno: " + std::string(strerror(errno)));
    int enable = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw Exception("enabling setsockopt(SO_REUSEADDR) failed with errno: " + std::string(strerror(errno)));
    if(bind(fd, ownAddress.getAddress(), ownAddress.getAddressLength()) < 0)
        throw Exception("binding socket failed with errno: " + std::string(strerror(errno)));
}

int common::UDPsocket::getFd() const
{
    return fd;
}

const common::Address &common::UDPsocket::getAddress() const
{
    return ownAddress;
}

void common::UDPsocket::setSendTimeout(unsigned milliseconds)
{
    timeval timeout = { (time_t)(milliseconds/1000), (time_t)(1000*(milliseconds%1000)) };
    if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
        throw Exception("setsockopt(SO_SNDTIMEO) failed with errno " + std::string(strerror(errno)));
}

void common::UDPsocket::setReceiveTimeout(unsigned milliseconds)
{
    timeval timeout = { (time_t)(milliseconds/1000), (time_t)(1000*(milliseconds%1000)) };
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
        throw Exception("setsockopt(SO_RCVTIMEO) failed with errno " + std::string(strerror(errno)));
}

common::UDPsocket::~UDPsocket()
{
    if(fd > 0)
        close(fd);
}
