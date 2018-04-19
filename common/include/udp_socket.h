#pragma once
#include <cstdint>
#include <netinet/in.h>

namespace common
{
    struct UDPsocket
    {
        int fd;
        sockaddr_in ownAddress; // will be changed to generic to support IPv6
        UDPsocket(uint16_t port);
        ~UDPsocket();
    };
}
