#pragma once
#include "udp_socket.h"
#include "address.h"
#include <map>
#include <functional>
#include <string>
#include <netdb.h>

namespace common
{
class UDPClient
{
public:
    class MessageCallback // interface used in function addToMessageQueue
    {
    public:
        virtual void callbackOnReceive(Address &address, const char *input_msg, size_t input_msg_length) = 0;
        virtual void callbackOnTimeout() = 0;
    };

    UDPClient(size_t input_buffer_size);
    UDPsocket &getSocket(); // non const, because socket options can be set
    const AddressInfo &getServerAddrinfo() const;

    // receive and send return number of received bytes or negative value on error.
    // Note that size should be max 508 bytes
    // These are 'low level' functions, see addToMessageQueue for another possibility
    int receive(char *buffer, size_t size); // receives from ANY address
    int send(const char *data, size_t size, Address &address); // sends to given address

    void addToMessageQueue(MessageCallback &callback, const Address &address,
                           const char *output_msg, size_t output_msg_length);
    void receiveAndCallCallbacks();

protected:
    UDPsocket socket;
    std::string inputBuffer;
    std::map<std::reference_wrapper<Address>, std::reference_wrapper<MessageCallback>> callbackMap;
};
}

