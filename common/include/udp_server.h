#pragma once
#include "udp_socket.h"
#include <netdb.h>

namespace common
{
class UDPServer
{
public:
    UDPServer(uint16_t port);
    const Address getClientAddress();

    // receive and send return number of received bytes or negative value on error.
    // Note that size should be max 508 bytes
    int receive(char *buffer, size_t size);
    int send(const char *data, size_t size);

private:
    UDPsocket socket;
    Address clientAddress;
    bool clientAddressIsCorrect;
};
}
