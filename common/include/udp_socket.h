#pragma once
#include "address.h"
#include <cstdint>
#include <netinet/in.h>

namespace common
{
    struct UDPsocket
    {
        int fd;
        Address ownAddress;
        UDPsocket(uint16_t port);
        ~UDPsocket();
    };
}
