//
// Created by T. Nowak
//

#include "address.h"
#include "exception.h"
#include  <arpa/inet.h>
#include <cstring>
#include <sstream>
#ifndef NDEBUG
#include <iostream>
#include "terminal_lock.h"
#endif

common::AddressInfo::AddressInfo(const char *host, const char *port, int socktype)
    : ai(nullptr)
{
    addrinfo hints;
    std::memset(&hints, 0, sizeof hints);
    if(isalpha(host[0])) // non-numeric address, for example "localhost" - prefer AF_INET6
        hints.ai_family = AF_INET6;
    hints.ai_socktype = socktype;
    int retval = getaddrinfo(host, port, &hints, &ai);
    if(retval != 0)
    {
        if(retval == EAI_SYSTEM)
            throw ExceptionInfo("getaddrinfo failed with errno: " + std::string(strerror(errno)));
        else
            throw ExceptionInfo("getaddrinfo failed with retval: " + std::string(gai_strerror(retval)));
    }
#ifndef NDEBUG
    if(getResult()->ai_next)
    {
        TerminalLock lock;
        std::cerr << "warning - many getaddrinfo results:\n";
        forEach([](addrinfo *ai) { Address(ai).print(std::cerr); });
    }
#endif
}

common::AddressInfo::~AddressInfo()
{
    if(ai)
        freeaddrinfo(ai);
}

common::AddressInfo::AddressInfo(common::AddressInfo &&other)
    : ai(other.ai)
{
    other.ai = nullptr;
}

const addrinfo* common::AddressInfo::getResult() const
{
    return ai;
}

common::Address::Address()
    : addressLength(sizeof(sockaddr_in6))
{
    // address remains uninitialized - it will be filled in, for example, with recvfrom
}

common::Address::Address(uint16_t port)
    : addressLength(sizeof(sockaddr_in6))
{
    std::memset(&address, 0, addressLength);
    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);
    address.sin6_addr = IN6ADDR_ANY_INIT;
}

common::Address::Address(const addrinfo *ai)
    : addressLength(ai->ai_addrlen)
{
    std::memcpy(&address, ai->ai_addr, addressLength);
}

common::Address::Address(const AddressInfo &ai)
    : Address(ai.getResult())
{}

common::Address::Address(const std::string &host, uint16_t port)
    : Address(AddressInfo(host.c_str(), std::to_string(port).c_str()).getResult())
{}

common::Address::Address(const std::string &host, const std::string &port)
    : Address(AddressInfo(host.c_str(), port.c_str()).getResult())
{}

sockaddr *common::Address::getAddress()
{
    return reinterpret_cast<sockaddr*>(&address);
}

const sockaddr *common::Address::getAddress() const
{
    return reinterpret_cast<const sockaddr*>(&address);
}

socklen_t common::Address::getAddressLength() const
{
    return addressLength;
}

socklen_t* common::Address::getAddressLengthPointer()
{
    return &addressLength;
}

uint16_t common::Address::getPort() const
{
    if(getAddress()->sa_family == AF_INET6)
        return ntohs(address.sin6_port);
    else if(getAddress()->sa_family == AF_INET)
        return ntohs(reinterpret_cast<const sockaddr_in*>(&address)->sin_port);
    else
        throw Exception("Address family (" + std::to_string(getAddress()->sa_family) + ") does not match neither IPV6 nor IPV4");
}

std::string common::Address::hostToString() const
{
    char address_str[INET6_ADDRSTRLEN];
    int retval = getnameinfo(getAddress(), addressLength, address_str, INET6_ADDRSTRLEN, 0, 0, NI_NUMERICHOST);
    if(retval != 0)
    {
        if(retval == EAI_SYSTEM)
            throw ExceptionInfo("getnameinfo failed with errno: " + std::string(strerror(errno)));
        else
            throw ExceptionInfo("getnameinfo failed with retval: " + std::string(gai_strerror(retval)));
    }
    if(strncmp(address_str, "::ffff:", 7) == 0)
        return std::string(address_str + 7); // ugly, but it is the only way to return IPv4 string from IPv6-mapped IPv4 address
    return std::string(address_str);
}

std::string common::Address::portToString() const
{
    return std::to_string(getPort());
}

std::string common::Address::toString() const
{
    return hostToString() + " :" + portToString();
}

void common::Address::print(std::ostream &os) const
{
    os << toString() << "\n";
}

bool common::Address::operator<(const common::Address &other) const
{
    return toString() < other.toString();
}

bool common::Address::operator==(const common::Address &other) const
{
    return toString() == other.toString();
}

bool common::Address::operator!=(const common::Address &other) const
{
    return !(*this == other);
}

bool common::Address::isLoopback(uint16_t port) const
{
    if(getAddress()->sa_family == AF_INET6)
    {
        int memcmp_retval = std::memcmp(&address.sin6_addr, &in6addr_loopback, sizeof(in6_addr));
        return ( memcmp_retval == 0 && ntohs(address.sin6_port) == port);
    }
    else
    {
        const sockaddr_in *ipv4 = reinterpret_cast<const sockaddr_in*>(&address);
        return (ipv4->sin_addr.s_addr == INADDR_LOOPBACK && ntohs(ipv4->sin_port) == port);
    }
}

void common::Address::incrementPort()
{
    if(getAddress()->sa_family == AF_INET6)
        address.sin6_port = htons(ntohs(address.sin6_port) + 1);
    else if(getAddress()->sa_family == AF_INET)
    {
        sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in*>(&address);
        ipv4->sin_port = htons(ntohs(ipv4->sin_port) + 1);
    }
    else
        throw Exception("Address family (" + std::to_string(getAddress()->sa_family) + ") does not match neither IPV6 nor IPV4");
}
