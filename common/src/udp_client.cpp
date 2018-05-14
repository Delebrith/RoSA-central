#include "udp_client.h"
#include "error_handler.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef DEBUG
#include <iostream>
#endif

#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)
#define ERROR(msg) ErrorHandler::getInstance().error(msg, __LINE__ ,__FILE__)

common::UDPClient::UDPClient(size_t input_buffer_size)
    : socket(0), inputBuffer(input_buffer_size, 0) // port 0 - bind socket to an ephemeral port
{}

common::UDPsocket &common::UDPClient::getSocket()
{
    return socket;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
    int retval = recvfrom(socket.getFd(), buffer, size, 0, 0, 0);
#ifdef DEBUG
    if(retval < 0)
        ERROR("no data received");
    else
        std::cout << "receive without saving address - ok\n";
#endif
    return retval;
}

int common::UDPClient::receive(char *buffer, size_t size, Address &address_to_fill_in)
{
    int retval = recvfrom(socket.getFd(), buffer, size, 0, address_to_fill_in.getAddress(),
                          address_to_fill_in.getAddressLengthPointer());
#ifdef DEBUG
    if(retval < 0)
        ERROR("no data received");
    else
    {
        std::cout << "client received answer from:\n";
        address_to_fill_in.print(std::cout);
    }
#endif
    return retval;
}

int common::UDPClient::send(const char *data, size_t size, const Address &address)
{
    int retval = sendto(socket.getFd(), data, size, 0, address.getAddress(), address.getAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

void common::UDPClient::addToMessageQueue(MessageCallback *callback, const common::Address &address,
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

#undef ERROR
#undef FATAL_ERROR
