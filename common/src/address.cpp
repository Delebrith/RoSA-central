#include "address.h"
#include "error_handler.h"
#include <cstring>
#ifdef DEBUG
#include <iostream>
#endif


#define FATAL_ERROR(msg) ErrorHandler::getInstance().fatalError(msg, __LINE__ ,__FILE__)

common::AddressInfo::AddressInfo(const char *host, const char *port, int socktype)
    : ai(nullptr)
{
    addrinfo hints;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = socktype;
    if(getaddrinfo(host, port, &hints, &ai) != 0)
        FATAL_ERROR("getaddrinfo");    
#ifdef DEBUG
    std::cout << "getaddrinfo result:\n";
    forEach([](addrinfo *ai) { Address(ai).print(std::cout); });
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
    : addressLength(ai.getResult()->ai_addrlen)
{
    std::memcpy(&address, ai.getResult()->ai_addr, addressLength);
}

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

void common::Address::print(std::ostream &os)
{
    static char address_str[INET6_ADDRSTRLEN];
    static char port_str[6];
    getnameinfo(getAddress(), addressLength, address_str, INET6_ADDRSTRLEN, port_str, 6, NI_NUMERICSERV);
    os << address_str << " :" << port_str << "\n";
}

bool common::Address::operator<(const common::Address &other) const
{
    if(addressLength != other.addressLength)
        return addressLength < other.addressLength;
    return std::memcmp(getAddress(), other.getAddress(), addressLength) < 0;
}

#undef FATAL_ERROR
