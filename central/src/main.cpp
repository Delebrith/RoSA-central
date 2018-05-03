#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "../include/RestService.h"

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


int main(int argc, char **argv)
{
    (void)argc; // unused
    (void)argv;

    
    //activate rest service
    utility::string_t port = U("8081");
    utility::string_t address = U("http://localhost:");
    address.append(port);
    on_initialize(address);


    char message[] = "hello";
    common::UDPClient client("localhost", "9000");
    std::cout << "UDP client started\n";
    int retval = client.send(message, sizeof message);
    if(retval < 0)
    {
        std::cout << "error, no data sent\n";
        return -1;
    }
    std::memset(message, 0, sizeof message); // clear buffer to see what we will receive
    retval = client.receive(message, sizeof message);
    if(retval < 0)
    {
        std::cout << "error, no data received\n";
        return -2;
    }
    std::cout << "Received: " << message << "\n";
    return 0;
}
