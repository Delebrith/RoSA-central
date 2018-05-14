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

const common::AddressInfo &common::UDPClient::getServerAddrinfo() const
{
   return serverAddressInfo;
}

int common::UDPClient::receive(char *buffer, size_t size)
{
    int retval;
#ifdef DEBUG
    Address client_address;
    retval = recvfrom(socket.getFd(), buffer, size, 0, client_address.getAddress(), client_address.getAddressLengthPointer());
#endif
    retval = recvfrom(socket.getFd(), buffer, size, 0, 0, 0);
    //TODO - chceck if addr is the same as the address we sent datagram to
    if(retval < 0)
        ERROR("no data received");
    else
    {
#ifdef DEBUG
        std::cout << "client received answer from:\n";
        client_address.print(std::cout);
#endif
    }
    return retval;
}

void common::UDPClient::addToMessageQueue(common::UDPClient::MessageCallback &callback, const common::Address &address, const char *output_msg, size_t output_msg_length)
{

}

void common::UDPClient::receiveAndCallCallbacks()
{

}

int common::UDPClient::send(const char *data, size_t size, Address &address)
{
    int retval = sendto(socket.getFd(), data, size, 0, address.getAddress(), address.getAddressLength());
    if(retval < 0) // handling of this error will be changed
        ERROR("failed to send data");
    return retval;
}

#undef ERROR
#undef FATAL_ERROR
