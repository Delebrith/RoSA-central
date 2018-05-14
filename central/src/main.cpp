#include <udp_client.h>
#include <iostream>
#include <cstring>

class Callback : public common::UDPClient::MessageCallback
{
public:
    Callback(const std::string str)
        : name(str)
    {}

    virtual void callbackOnReceive(common::Address &address, char *msg, size_t length)
    {
        msg[length] = '\0';
        std::cout << name << " - received: '" << msg << "'\nfrom: ";
        address.print(std::cout);
        std::cout << std::endl;
    }

    virtual void callbackOnError()
    {
        std::cout << "callbackOnError\n";
    }

private:
    std::string name;
};

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port1> <port2>\n";
        return -1;
    }
    common::UDPClient client(512);
    client.getSocket().setSendTimeout(5000);
    client.getSocket().setReceiveTimeout(5000);
    std::cout << "UDP client started\n";
    common::Address server_address1(common::AddressInfo(argv[1], argv[2], SOCK_DGRAM).getResult());
    common::Address server_address2(common::AddressInfo(argv[1], argv[3], SOCK_DGRAM).getResult());
    Callback callback1("callback1"), callback2("callback2");
    client.addToMessageQueue(&callback1, server_address1, "hello1", sizeof("hello1"));
    client.addToMessageQueue(&callback2, server_address2, "hello2", sizeof("hello2"));
    std::cout << "Sent messages\n";
    client.receiveAndCallCallbacks();
    return 0;
}
