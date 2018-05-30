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
    int disable = 0;
    if(setsockopt(fd, SOL_SOCKET, IPV6_V6ONLY, &disable, sizeof(int)) < 0) // it MIGHT be unsupported - only waring on fail
        ExceptionInfo::warning("disabling setsockopt(IPV6ONLY) failed with errno: " + std::string(strerror(errno)));
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

void common::UDPsocket::setSendTimeout(unsigned milliseconds)
{
    timeval timeout = { milliseconds/1000, 1000*(milliseconds%1000) };
    if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
        throw Exception("setsockopt(SO_SNDTIMEO) failed with errno " + std::string(strerror(errno)));
}

void common::UDPsocket::setReceiveTimeout(unsigned milliseconds)
{
    timeval timeout = { milliseconds/1000, 1000*(milliseconds%1000) };
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
        throw Exception("setsockopt(SO_RCVTIMEO) failed with errno " + std::string(strerror(errno)));
}

common::UDPsocket::~UDPsocket()
{
    if(fd > 0)
        close(fd);
}
