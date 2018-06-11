//
// Created by T. Nowak
//

#pragma once
#include "udp_socket.h"
#include "address.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <netdb.h>

namespace common
{
class UDPClient
{
public:
    class Callback // interface used in function sendAndSaveCallback
    {
    public:
        virtual void callbackOnReceive(const Address &addr, std::string msg) = 0;
        virtual ~Callback() {}
    };

    // Constructor parameters:
    // - port - port from which client is going to send (port at which client receives will also be reserved - port+1)
    // - input_buffer_size - used in private receive function, should be the size of longest possible packet, however
    //   it should not be greater than min UDP packet size that won't get divided (max 508 bytes?)
    //   if input_buffer_size is 0, only function send should be used, because you will not receive any answer - callbacks are pointless
    // - default_callback - see comment above sendAndSaveCallback
    UDPClient(uint16_t port, size_t input_buffer_size, std::unique_ptr<Callback> &&default_callback = nullptr);

    // note that receive_socket must be bound to port 1 greater than send_socket
    UDPClient(UDPsocket &send_socket, UDPsocket &receive_socket, size_t input_buffer_size, std::unique_ptr<Callback> &&default_callback = nullptr);

    // destructor sends special message to the receiver thread
    ~UDPClient();

    UDPClient(const UDPClient &) = delete;
    void operator=(const UDPClient &) = delete;

    // send returns number of sent bytes or negative value on error.
    // Note that size should be small enough (max 508 bytes?) so that message fits into one UDP packet.
    // This is a 'low level' function that does not have to be used, see sendAndSaveCallback for another possibility.
    // Function send is thread-safe - each call is in a critical section.
    int send(const char *data, size_t size, const Address &address) const;

    // sendAndAddCallback sends given string. If sending fails, an exception is thrown.
    // If sending succeeds (which should happen most of the time), pair (address, callback) is saved in callbackMap.
    // Another threa (receiving messages) will call callback after receiving message from address in the map.
    // If a message comes from an unknown address, default callback (passed previously to the constructor) is called.
    void sendAndSaveCallback(const std::string &message, const Address &address, std::unique_ptr<Callback> &&callback);

protected:
    const UDPsocket sendSocket;
    const UDPsocket receiveSocket;
    std::vector<char> inputBuffer;
    std::unordered_map<const Address, std::unique_ptr<Callback>> callbackMap;
    const std::unique_ptr<Callback> defaultCallback;
    mutable std::mutex callbackMapMutex;
    mutable std::mutex sendSocketMutex;
    std::thread receiverThread;

    int receive(char *buffer, size_t size, Address &address_to_fill_in); // receives from ANY address, saving it, used only privately
    bool handleReceiveAndCheckIfEnd(const Address &addr, std::string message); // used in receiverThreadFunction, returns true if thread should end
    void receiverThreadFunction(); // runs receive in an "infinite" loop (it ends only in UDPClient destructor)
};
}

