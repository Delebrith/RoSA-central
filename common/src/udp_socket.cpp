#include "udp_socket.h"
#include "error_handler.h"
#include <unistd.h>

#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)

common::UDPsocket::UDPsocket(uint16_t port)
    : fd(-1)
{
    ownAddress.sin_family = AF_INET; // support for AF_INET6 will be added
    ownAddress.sin_addr.s_addr = INADDR_ANY;
    ownAddress.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
        FATAL_ERROR("creating socket");
    if(bind(fd, (sockaddr*) &ownAddress, sizeof ownAddress) < 0)
        FATAL_ERROR("binding socket");
}

common::UDPsocket::~UDPsocket()
{
    if(fd > 0)
        close(fd);
}
