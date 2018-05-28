#include <udp_client.h>
#include <iostream>
#include <cstring>
#include <thread>
#include "RestService.h"
#include "SensorList.h"
#include "ScriptExecutor.h"

SensorList sensorList;

/*
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

std::unique_ptr<RestService, std::default_delete<RestService>> rest;
void on_initialize(const string_t& address)
{
    // Build our listener's URI from the configured address and the hard-coded path "MyServer/Action"

    uri_builder sensorUri(address);
    sensorUri.append_path(U(RestService::base_uri));

    auto addr = sensorUri.to_uri().to_string();
    rest = std::unique_ptr<RestService>(new RestService(addr));
    rest->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

    return;
}

void activate_rest_service()
{
    utility::string_t port = U("8081");
    utility::string_t address = U("http://localhost:");
    address.append(port);
    on_initialize(address);
}

class Callback : public common::UDPClient::Callback
{
public:
    Callback(const std::string str)
        : name(str)
    {}

    virtual void callbackOnReceive(common::Address &address, char *msg, size_t length)
    {
        if(length < 512)
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

void test_udp_client(int argc, char **argv)
{
    std::cout << "\nTesting UDPClient...\n";
    if(argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port1> <port2>\n";
        return;
    }
    common::Address server_address1(common::AddressInfo(argv[1], argv[2], SOCK_DGRAM).getResult());
    common::Address server_address2(common::AddressInfo(argv[1], argv[3], SOCK_DGRAM).getResult());
    Callback callback1("callback1"), callback2("callback2"), default_callback("default callback");
    common::UDPClient client(512, &default_callback);
    client.getSocket().setSendTimeout(5000);
    client.getSocket().setReceiveTimeout(5000);
    std::cout << "UDP client started\n";
    client.addToMessageQueue(&callback1, server_address1, "hello1", sizeof("hello1"));
    client.addToMessageQueue(&callback2, server_address2, "hello2", sizeof("hello2"));
    std::cout << "Sent messages\n";
    client.receiveAndCallCallbacks();
}*/
void execute() {
    ScriptExecutor executor(&sensorList);
    executor.execute();
}

int main(int argc, char **argv)
{
    //activate_rest_service();
    //test_udp_client(argc, argv);
    std::thread t1(execute);
    t1.join();
    return 0;
}
