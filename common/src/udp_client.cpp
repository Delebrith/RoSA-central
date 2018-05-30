#include "udp_client.h"
#include "exception.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef NDEBUG
#include <iostream>
#endif

common::UDPClient::UDPClient(size_t input_buffer_size)
    : socket(0), inputBuffer(input_buffer_size, 0), defaultCallback(nullptr) // port 0 - bind socket to an ephemeral port
{}

common::UDPClient::UDPClient(size_t input_buffer_size, Callback *default_callback)
    : socket(0), inputBuffer(input_buffer_size, 0), defaultCallback(default_callback) // port 0 - bind socket to an ephemeral port
{}

void common::UDPClient::setDefaultCallback(common::UDPClient::Callback *default_callback)
{
    defaultCallback = default_callback;
}

common::UDPsocket &common::UDPClient::getSocket()
{
    return socket;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
    int retval = recvfrom(socket.getFd(), buffer, size, 0, 0, 0);
    if(retval < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // non-standard error (not timeout / resource unavailability)
        ExceptionInfo::warning("receive failed with errno: " + std::string(strerror(errno)));
    return retval;
}

int common::UDPClient::receive(char *buffer, size_t size, Address &address_to_fill_in)
{
    int retval = recvfrom(socket.getFd(), buffer, size, 0, address_to_fill_in.getAddress(),
                          address_to_fill_in.getAddressLengthPointer());
    if(retval < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // non-standard error (not timeout / resource unavailability)
        ExceptionInfo::warning("receive failed with errno: " + std::string(strerror(errno)));
#ifndef NDEBUG
    if(retval > 0)
    {
        std::cerr << "client received answer from: ";
        address_to_fill_in.print(std::cerr);
    }
#endif
    return retval;
}

int common::UDPClient::send(const char *data, size_t size, const Address &address)
{
    int retval = sendto(socket.getFd(), data, size, 0, address.getAddress(), address.getAddressLength());
    if(retval < 0) // sending UDP packet should generally not fail, even if packet will not be received
        ExceptionInfo::warning("send failed with errno: " + std::string(strerror(errno)));
    return retval;
}

void common::UDPClient::addToMessageQueue(Callback *callback, const common::Address &address,
                                          const char *output_msg, size_t output_msg_length)
{
    if(send(output_msg, output_msg_length, address) > 0)
        callbackMap[address] = callback;
}

void common::UDPClient::receiveAndCallCallbacks()
{
    Address addr;
    while(!callbackMap.empty())
    {
        int retval = receive(inputBuffer.data(), inputBuffer.size(), addr);
        if(retval > 0)
        {
            auto it = callbackMap.find(addr);
            if(it == callbackMap.end())
            {
                if(defaultCallback != nullptr)
                    defaultCallback->callbackOnReceive(addr, reinterpret_cast<char*>(inputBuffer.data()), retval);
                continue;
            }
#ifndef NDEBUG
            if(it->second == nullptr) // it can happen only due to programmer error, so in Release this check will be disabled
                throw Exception("callback in callbackMap is nullptr");
#endif
            it->second->callbackOnReceive(addr, reinterpret_cast<char*>(inputBuffer.data()), retval);
            callbackMap.erase(it);
        }
        else
        {
            for(auto &it : callbackMap)
                it.second->callbackOnError();
            callbackMap.clear();
        }
    }
}
