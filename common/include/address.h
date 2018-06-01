#pragma once
#include <netdb.h>
#include <ostream>
#include <cstring>

namespace common
{
class AddressInfo
{
public:
    AddressInfo(const char *host, const char *port, int socktype = SOCK_DGRAM);
    ~AddressInfo();
    AddressInfo(const AddressInfo &) = delete;
    void operator=(const AddressInfo &) = delete;
    AddressInfo(AddressInfo &&other);
    const addrinfo* getResult() const;

    template<typename Functor>
    void forEach(Functor &f) const
    {
        for(addrinfo *tmp = ai; tmp != nullptr; tmp = tmp->ai_next)
            f(tmp);
    }

    template<typename Functor>
    void forEach(const Functor &f) const
    {
        for(addrinfo *tmp = ai; tmp != nullptr; tmp = tmp->ai_next)
            f(tmp);
    }

private:
    addrinfo *ai;
};

class Address
{
public:
    // constructors:
    Address();
    Address(uint16_t port);
    Address(const addrinfo *ai);
    Address(const AddressInfo &ai);
    Address(const std::string &host, uint16_t port);
    Address(const std::string &host, const std::string &port);

    // getters of raw data:
    sockaddr* getAddress();
    const sockaddr* getAddress() const;
    socklen_t getAddressLength() const;
    socklen_t* getAddressLengthPointer();
    uint16_t getPort() const;

    // "string" getters:
    std::string hostToString() const;
    std::string portToString() const;
    std::string toString() const;
    void print(std::ostream &os) const;

    // operators, allowing Addresses to be stored in some ordered data structure
    bool operator<(const Address &other) const; // enables to use Addresses as map keys
    bool operator==(const Address &other) const;
    bool operator!=(const Address &other) const;

    // 'strange' functions, required by speficic implementation details of other parts of the system:
    // isLocalhost is used in UDPClient to check if a special message from loopback address was received
    bool isLoopback(uint16_t port) const;
    // incrementPort is used in UDPServer (it sends answer to port from which message was received + 1,
    // so that the from client's point of view, sending requests and receiving answers can be independant)
    void incrementPort();

private:
    sockaddr_in6 address;
    socklen_t addressLength;
};
}

namespace std
{
template <>
struct hash<const common::Address>
{
    size_t operator()(const common::Address &a) const
    {
        size_t retval;
        constexpr unsigned halfsize = sizeof(retval) / 2;
        std::memcpy(&retval, a.getAddress()->sa_data, halfsize); // copy first halfsize bytes of adddress data to hash
        std::memcpy((char*)&retval + halfsize, a.getAddress()->sa_data + sizeof(a.getAddress()->sa_data) - halfsize, halfsize); // copy last halfsize bytes
        return retval;
    }
};
}
