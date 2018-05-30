#include "address.h"
#include "exception.h"
#include <cstring>
#include <sstream>
#ifndef NDEBUG
#include <iostream>
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
    std::cerr << "getaddrinfo result:\n";
    forEach([](addrinfo *ai) { Address(ai).print(std::cerr); });
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
    address.sin6_addr = in6addr_any;
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

std::string common::Address::hostToString() const
{
    char address_str[INET6_ADDRSTRLEN];
    getnameinfo(getAddress(), addressLength, address_str, INET6_ADDRSTRLEN, 0, 0, NI_NUMERICSERV);
    return std::string(address_str, INET6_ADDRSTRLEN);
}

std::string common::Address::portToString() const
{
    char port_str[6]; // max port number has 5 digits
    getnameinfo(getAddress(), addressLength, 0, 0, port_str, 6, NI_NUMERICSERV);
    return std::string(port_str, 6);
}

std::string common::Address::toString() const
{
    std::ostringstream oss;
    char address_str[INET6_ADDRSTRLEN];
    char port_str[6];
    getnameinfo(getAddress(), addressLength, address_str, INET6_ADDRSTRLEN, port_str, 6, NI_NUMERICSERV);
    oss << address_str << " :" << port_str;
    return oss.str();
}

uint16_t common::Address::port() const
{
    if(getAddress()->sa_family == AF_INET6)
        return ntohs(address.sin6_port);
    else if(getAddress()->sa_family == AF_INET)
        return ntohs(reinterpret_cast<const sockaddr_in*>(&address)->sin_port);
    else
        throw Exception("Address family (" + std::to_string(getAddress()->sa_family) + ") does not match neither IPV6 nor IPV4");
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

void common::Address::print(std::ostream &os) const
{
    os << toString() << "\n";
}

bool common::Address::operator<(const common::Address &other) const
{
    if(addressLength != other.addressLength)
        return addressLength < other.addressLength;
    return std::memcmp(getAddress(), other.getAddress(), addressLength) < 0;
}

bool common::Address::operator==(const common::Address &other) const
{
    if(addressLength != other.addressLength)
        return false;
    return std::memcmp(getAddress(), other.getAddress(), addressLength) == 0;
}

bool common::Address::operator!=(const common::Address &other) const
{
    return !(*this == other);
}
