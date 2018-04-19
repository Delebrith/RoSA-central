#pragma once
#include "udp_socket.h"
#include <netdb.h>

namespace common
{
class UDPClient
{
public:
    UDPClient(const char *host, const char *port);
    ~UDPClient();
    const addrinfo* getServerAddrinfo();

    // receive and send return number of received bytes or negative value on error.
    // Note that size should be max 508 bytes
    int receive(char *buffer, size_t size);
    int send(const char *data, size_t size);

private:
    UDPsocket socket;
    addrinfo *serverAddrinfo;
};
}

