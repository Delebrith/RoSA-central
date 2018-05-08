#pragma once
#include "address.h"
#include <cstdint>
#include <netinet/in.h>

namespace common
{
    class UDPsocket
    {
    public:
        UDPsocket(uint16_t port);
        int getFd() const;
        void setSendTimeout(unsigned milliseconds);
        void setReceiveTimeout(unsigned milliseconds);
        ~UDPsocket();

    private:
        int fd;
        Address ownAddress;
    };
}
