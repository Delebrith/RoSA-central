#pragma once
#include <netdb.h>
#include <ostream>

namespace common
{
class AddressInfo
{
public:
    AddressInfo(const char *host, const char *port, int socktype);
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
    sockaddr* getAddress();
    const sockaddr* getAddress() const;
    socklen_t getAddressLength() const;
    socklen_t* getAddressLengthPointer();
    void print(std::ostream &os);
    bool operator<(const Address &other); // enables to use Addresses as map keys

private:
    sockaddr_in6 address;
    socklen_t addressLength;
};
}
