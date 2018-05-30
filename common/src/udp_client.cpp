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

common::UDPClient::UDPClient(uint16_t port, size_t input_buffer_size, std::unique_ptr<Callback> default_callback)
    : sendSocket(port), receiveSocket(port + 1), inputBuffer(input_buffer_size, 0), defaultCallback(default_callback)
{}

common::UDPClient::UDPClient(common::UDPsocket &send_socket, common::UDPsocket &receive_socket, size_t input_buffer_size, std::unique_ptr<common::UDPClient::Callback> default_callback)
    : sendSocket(send_socket), receiveSocket(receive_socket), inputBuffer(input_buffer_size, 0), defaultCallback(default_callback)
{
    if(receiveSocket.getAddress().port() - sendSocket.getAddress().port() != 1)
        throw Exception("receiveSocket in UDPClient must be bound at port 1 greater than sendSocket");
}

common::UDPClient::~UDPClient()
{
    char magic = 255;
    // if receiverThreadFunction gets such message from address to which sendSocket is bound, thread stops receiving
    send(&magic, 1, receiveSocket.getAddress());
    receiverThread.join();
}

int common::UDPClient::send(const char *data, size_t size, const Address &address)
{
    int retval;
    {
        std::unique_lock lock(sendSocketMutex);
        retval = sendto(sendSocket.getFd(), data, size, 0, address.getAddress(), address.getAddressLength());
    }
    return retval;
}

void common::UDPClient::sendAndSaveCallback(const std::string &message, const common::Address &address, std::unique_ptr<common::UDPClient::Callback> callback)
{
    int retval = send(message.c_str(), message.length(), address);
    if(retval < 0)
        throw ExceptionInfo("send to address " + address.toString() + " failed with retval: " + std::to_string(retval) + " and errno: " + std::string(strerror(errno)));
    {
        std::unique_lock lock(callbackMapMutex);
        auto it = callbackMap.find(address);
        if(it == callbackMap.end())
            throw ExceptionInfo("callback for address " + address.toString() + " not added to the map - it already exists");
        callbackMap.emplace_hint(it, )
    }
}

void common::UDPClient::addToMessageQueue(Callback *callback, const common::Address &address,
                                          const char *output_msg, size_t output_msg_length)
{
    if(send(output_msg, output_msg_length, address) > 0)
        callbackMap[address] = callback;
}

int common::UDPClient::receive(char *buffer, size_t size, Address &address_to_fill_in)
{
    int retval = recvfrom(receiveSocket.getFd(), buffer, size, 0, address_to_fill_in.getAddress(),
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

void common::UDPClient::receiverThreadFunction()
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
                ExceptionInfo::warning("callback in callbackMap for address " + it->first->toString() + " is nullptr");
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
