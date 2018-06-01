#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "RestService.h"
#include "ScriptExecutor.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

/*
std::unique_ptr<RestService, std::default_delete<RestService>> rest;

void on_initialize(const string_t& address)
{
    // Build our listener's URI from the configured address and the hard-coded path "MyServer/Action"

    uri_builder sensorUri(address);
    sensorUri.append_path(U(RestService::base_uri));

    auto addr = sensorUri.to_uri().to_string();
    rest = std::unique_ptr<RestService>(new RestService(addr, &sensorlist));
    rest->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

    return;
}

void activate_rest_service(const utility::string_t host)
{
    utility::string_t port = U(":8081");
    utility::string_t address = U(U("http://") + host);
    address.append(port);
    on_initialize(address);
}

void close_rest_service()
{
    rest->close().wait();
    return;
}*/

class Callback : public common::UDPClient::Callback
{
public:
    Callback(const std::string str)
            : name(str) {}

    virtual void callbackOnReceive(const common::Address &address, std::string msg) {
        static std::hash<const common::Address> hasher;
        std::cout << name << " - received: '" << msg << "'\nfrom: ";
        address.print(std::cout);
        std::cout << "(address hash: " << hasher(address) << ")\n";
        std::cout << std::endl;
    }

private:
    std::string name;
};

void execute(Communicator *communicator) {
    ScriptExecutor executor(communicator);
    executor.execute();
}

void test_udp_client() {
    Communicator communicator;
    execute(&communicator);
    return;
}


int main(int argc, char **argv)
{
    try
    {
        //activate_rest_service(U(argv[1]));
        test_udp_client();
        std::cout << "press enter to exit...";
        while (std::cin.get() != '\n')
        {
            continue;
        }
        //close_rest_service();
        exit(0);
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}
