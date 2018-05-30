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
    AddressInfo(const AddressInfo &other) = delete;
    void operator=(const AddressInfo &other) = delete;
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
    Address();
    Address(uint16_t port);
    Address(const addrinfo *ai);
    Address(const AddressInfo &ai);
    Address(const std::string &host, uint16_t port);
    Address(const std::string &host, const std::string &port);
    sockaddr* getAddress();
    const sockaddr* getAddress() const;
    socklen_t getAddressLength() const;
    socklen_t* getAddressLengthPointer();
    std::string hostToString() const;
    std::string portToString() const;
    std::string toString() const;
    uint16_t port() const;
    void print(std::ostream &os);
    bool operator<(const Address &other) const; // enables to use Addresses as map keys
    bool operator==(const Address &other) const;

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
