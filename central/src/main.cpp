#include <udp_client.h>
#include <udp_server.h>
#include <iostream>
#include <cstring>
#include "RestService.h"
#include "ScriptExecutor.h"
#include "Communicator.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;


SensorList sensorList;
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

void executeScripts(Communicator *communicator) {
    ScriptExecutor executor(communicator);
    executor.execute();
}

void server() {
    try {
        char buffer[512];
        common::UDPServer server(6000);
        server.getSocket().setSendTimeout(2000);
        std::cout << "UDP server started\n";
        while (true) {
            int retval = server.receive(buffer, 511);
            if (retval < 0) {
                std::cout << "error, no data received\n";
                return;
            }

            //Todo: rozpisać wpisywanie do listy


            std::cout << "Received: " << buffer << "\n";
            if (server.send(buffer, retval) > 0)
                std::cout << "Sent answer\n";
            else
                std::cout << "Failed to send answer\n";
            memset(buffer, 0, sizeof(buffer));
        }
    }
    catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return;
    }
}



int main(int argc, char **argv)
{
    try
    {
        //activate_rest_service(U(argv[1]));
        Communicator communicator(&sensorList);
        executeScripts(&communicator);
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
