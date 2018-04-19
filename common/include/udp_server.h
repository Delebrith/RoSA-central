#pragma once
#include "udp_socket.h"
#include <netdb.h>

namespace common
{
class UDPServer
{
public:
    UDPServer(uint16_t port);
    const sockaddr* getClientAddress();
    socklen_t getClientAddressLength();


    // receive and send return number of received bytes or negative value on error.
    // Note that size should be max 508 bytes
    int receive(char *buffer, size_t size);
    int send(const char *data, size_t size);

private:
    UDPsocket socket;
    sockaddr clientAddress;
    socklen_t clientAddressLength;
    bool clientAddressIsCorrect;
};
}
