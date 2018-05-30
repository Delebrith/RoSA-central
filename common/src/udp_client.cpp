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
#include <functional>

common::UDPClient::UDPClient(uint16_t port, size_t input_buffer_size, std::unique_ptr<Callback> &&default_callback)
    : sendSocket(port), receiveSocket(port + 1), inputBuffer(input_buffer_size, 0), defaultCallback(std::move(default_callback)),
      receiverThread(&common::UDPClient::receiverThreadFunction, std::ref(*this))
{}

common::UDPClient::UDPClient(common::UDPsocket &send_socket, common::UDPsocket &receive_socket, size_t input_buffer_size, std::unique_ptr<common::UDPClient::Callback> &&default_callback)
    : sendSocket(send_socket), receiveSocket(receive_socket), inputBuffer(input_buffer_size, 0), defaultCallback(std::move(default_callback)),
      receiverThread(&common::UDPClient::receiverThreadFunction, std::ref(*this))
{
    if(receiveSocket.getAddress().port() - sendSocket.getAddress().port() != 1)
        throw Exception("receiveSocket in UDPClient must be bound at port 1 greater than sendSocket");
}

common::UDPClient::~UDPClient()
{
    char magic = -1;
    // if receiverThreadFunction gets such message from address to which sendSocket is bound, thread stops receiving
#ifndef NDEBUG
    std::cerr << "UDPClient destructor... ";
#endif
    if(send(&magic, 1, receiveSocket.getAddress()) < 0)
        ExceptionInfo::warning("CRITICAL ERROR - could not send kill message to receiver thread - the program might lock down");
    receiverThread.join();
#ifndef NDEBUG
    std::cerr << "ok\n";
#endif
}

int common::UDPClient::send(const char *data, size_t size, const Address &address) const
{
    int retval;
    { // sendSocketMutex critical section
        std::lock_guard<std::mutex> lock(sendSocketMutex);
        retval = sendto(sendSocket.getFd(), data, size, 0, address.getAddress(), address.getAddressLength());
    }
    return retval;
}

void common::UDPClient::sendAndSaveCallback(const std::string &message, const common::Address &address, std::unique_ptr<Callback> &&callback)
{
    int retval = send(message.c_str(), message.length(), address);
    if(retval < 0)
        throw ExceptionInfo("send to address " + address.toString() + " failed with retval: " + std::to_string(retval) + " and errno: " + std::string(strerror(errno)));
    { // callbackMapMutex critical section
        std::lock_guard<std::mutex> lock(callbackMapMutex);
        auto it = callbackMap.find(address);
        if(it != callbackMap.end())
        {
            ExceptionInfo::warning("callback for address " + address.toString() + " overwritten in the map");
            it->second = std::move(callback);
        }
        else
            callbackMap.emplace_hint(it, address, std::move(callback));
    }
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

bool common::UDPClient::handleReceiveAndCheckIfEnd(const common::Address &addr, std::string message)
{
    bool found;
    std::unique_ptr<Callback> callback;
    { // callbackMapMutex critical section
        std::lock_guard<std::mutex> lock(callbackMapMutex);
        auto it = callbackMap.find(addr);
        found = (it != callbackMap.end());
        if(found)
        {
            callback = std::move(it->second);
            callbackMap.erase(it);
        }
    }
    if(found)
    {
#ifndef NDEBUG
        if(callback == nullptr) // it can happen only due to programmer error, so in Release this check will be disabled
        {
            ExceptionInfo::warning("callback in callbackMap for address " + addr.toString() + " is nullptr");
            return false;
        }
#endif
        callback->callbackOnReceive(addr, message);
    }
    else if(addr != sendSocket.getAddress())
    {
        std::cerr << "addresses " << addr.toString() << " and " << sendSocket.getAddress().toString() << " differ\n";
    }

//    else if(addr == sendSocket.getAddress()) // 'magic' message - from sending socket of the same class instance
//        return message[0] == -1; // the only place in which this function can return true
//    else if(defaultCallback.get() != nullptr)
//        defaultCallback->callbackOnReceive(addr, message);
    return false;
}

void common::UDPClient::receiverThreadFunction()
{
    Address addr;
    bool receiverThreadShouldEnd = false;
    while(!receiverThreadShouldEnd)
    {
        int retval = receive(inputBuffer.data(), inputBuffer.size(), addr); // this CAN block
        if(retval > 0) // otherwise - receive prints warning, no exception is thrown because this is auxiliary thread
            receiverThreadShouldEnd = handleReceiveAndCheckIfEnd(addr, std::string(inputBuffer.data(), retval));
    }
}
