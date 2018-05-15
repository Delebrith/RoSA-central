#pragma once
#include "udp_socket.h"
#include "address.h"
#include <map>
#include <functional>
#include <vector>
#include <netdb.h>

namespace common
{
class UDPClient
{
public:
    class Callback // interface used in function addToMessageQueue
    {
    public:
        virtual void callbackOnReceive(Address &address, char *input_msg, size_t input_msg_length) = 0;
        virtual void callbackOnError() = 0;
        virtual ~Callback() {}
    };

    UDPClient(size_t input_buffer_size);
    UDPClient(size_t input_buffer_size, Callback *default_callback); // default_callback - see receiveAndCallCallbacks()
    void setDefaultCallback(Callback *default_callback);
    UDPsocket &getSocket(); // non const reference, because socket options can be set

    // receive and send return number of received bytes or negative value on error.
    // Note that size should be small enough (max 508 bytes?) so that message comes in one UDP packet.
    // These are 'low level' functions that do not have to be used, see addToMessageQueue for another possibility.
    int receive(char *buffer, size_t size); // receives from ANY address, not saving it
    int receive(char *buffer, size_t size, Address &address_to_fill_in); // receives from ANY address, saving it
    int send(const char *data, size_t size, const Address &address);

    // addToMessageQueue sends output_msg of length output_msg_length to given address and saves pair (address, callback),
    // which will be used in receiveAndCallCallbacks(). It iteratively tries to collect responses to all messages in
    // the queue. If a response comes, corresponding callbackOnReceive() is called. If receive() fails, corresponding
    // callbackOnError() functions are called. If message comes from unknown address, defaultCallback is called (or message
    // is ignored - if defaultCallback is nullptr). Note that receiveAndCallCallbacks uses private buffer inside this class.
    void addToMessageQueue(Callback *callback, const Address &address,
                           const char *output_msg, size_t output_msg_length);
    void receiveAndCallCallbacks();

protected:
    UDPsocket socket;
    std::vector<char> inputBuffer;
    std::map<const Address, Callback*> callbackMap;
    Callback *defaultCallback;
};
}

