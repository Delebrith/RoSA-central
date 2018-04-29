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

private:
    sockaddr_in6 address;
    socklen_t addressLength;
};
}
