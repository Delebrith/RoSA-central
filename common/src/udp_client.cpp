//
// Created by T. Nowak
//

#include "udp_client.h"
#include "exception.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef NDEBUG
#include <iostream>
#include "terminal_lock.h"
#endif
#include <functional>

common::UDPClient::UDPClient(uint16_t port, size_t input_buffer_size, std::unique_ptr<Callback> &&default_callback)
    : sendSocket(port), receiveSocket(port + 1), inputBuffer(input_buffer_size, 0), defaultCallback(std::move(default_callback)),
      receiverThread(&common::UDPClient::receiverThreadFunction, std::ref(*this)) {
    if (input_buffer_size == 0) {
        ExceptionInfo::warning("initialized UDPClient without buffer - you will never receive answer to what you send");
        receiverThread.join();
#ifndef NDEBUG
        TerminalLock(), std::cerr << "receiverThread joined\n";
#endif
    }
}

common::UDPClient::UDPClient(common::UDPsocket &send_socket, common::UDPsocket &receive_socket, size_t input_buffer_size, std::unique_ptr<common::UDPClient::Callback> &&default_callback)
    : sendSocket(send_socket), receiveSocket(receive_socket), inputBuffer(input_buffer_size, 0), defaultCallback(std::move(default_callback)),
      receiverThread(&common::UDPClient::receiverThreadFunction, std::ref(*this))
{
    if (input_buffer_size == 0) {
        ExceptionInfo::warning("initialized UDPClient without buffer - you will never receive answer to what you send");
        receiverThread.join();
#ifndef NDEBUG
        TerminalLock(), std::cerr << "receiverThread joined\n";
#endif
    }
    if (receiveSocket.getAddress().getPort() - sendSocket.getAddress().getPort() != 1)
        throw Exception("receiveSocket in UDPClient must be bound at port 1 greater than sendSocket");
}

common::UDPClient::~UDPClient()
{
    char magic_msg = 0;
    // if receiverThreadFunction gets such message from address to which sendSocket is bound, thread stops receiving
#ifndef NDEBUG
    TerminalLock(), std::cerr << "UDPClient destructor... ";
#endif
    if (inputBuffer.size() != 0) // if it is equal 0, receiver thread does nothing and was already joined in constructor
    {
        if (send(&magic_msg, 1, receiveSocket.getAddress()) < 0)
            ExceptionInfo::warning(
                    "CRITICAL ERROR - could not send kill message to receiver thread - the program might lock down");
        receiverThread.join();
#ifndef NDEBUG
        TerminalLock(), std::cerr << "receiverThread joined\n";
#endif
    }
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
            it->second = std::move(callback);
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
        TerminalLock(), std::cerr << "client received answer from: ";
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
    if(found && callback != nullptr)
        callback->callbackOnReceive(addr, message);
    else if(message[0] == 0 && addr.isLoopback(sendSocket.getAddress().getPort()))
        return true; // end=true if 'magic' message came from sendSocket of the same class instance
    else if (defaultCallback.get() != nullptr)
        defaultCallback->callbackOnReceive(addr, message);
    return false;
}

void common::UDPClient::receiverThreadFunction()
{
    if (inputBuffer.size() == 0)
        return;
    Address addr;
    bool receiverThreadShouldEnd = false;
    while(!receiverThreadShouldEnd)
    {
        int retval = receive(inputBuffer.data(), inputBuffer.size(), addr); // this CAN block
        if(retval > 0) // otherwise - receive prints warning, no exception is thrown because this is auxiliary thread
            receiverThreadShouldEnd = handleReceiveAndCheckIfEnd(addr, std::string(inputBuffer.data(), retval));
    }
}
