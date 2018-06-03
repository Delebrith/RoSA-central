#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "SessionList.h"
#include "SensorList.h"
#include "WebServer.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

SensorList sensorList;
SessionList sessionList;
HttpServer server;


class Callback : public common::UDPClient::Callback
{
public:
    Callback(const std::string str)
        : name(str)
    {}

    virtual void callbackOnReceive(const common::Address &address, std::string msg)
    {
        static std::hash<const common::Address> hasher;
        std::cout << name << " - received: '" << msg << "'\nfrom: ";
        address.print(std::cout);
        std::cout << "(address hash: " << hasher(address) << ")\n";
        std::cout << std::endl;
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
    common::Address server_address1(argv[1], argv[2]);
    common::Address server_address2(argv[1], argv[3]);

    std::unique_ptr<common::UDPClient::Callback> default_callback = std::unique_ptr<common::UDPClient::Callback>(new Callback("default_callback"));
    static common::UDPClient client(6000, 512, std::move(default_callback));
    std::cout << "UDP client started\n";
    auto send_message_thread = [&](const common::Address &addr, const std::string msg) {
        client.sendAndSaveCallback(msg, addr, std::unique_ptr<common::UDPClient::Callback>(new Callback("callback(" + msg + ")")));
    };
    std::thread thread1(send_message_thread, std::ref(server_address1), "hello1");
    std::thread thread2(send_message_thread, std::ref(server_address2), "hello2");
    thread1.join();
    thread2.join();
}

int main(int argc, char **argv)
{

    WebServer webServer(&sessionList, &sensorList, &server);
    std::cout << "web server created...\n";

    thread server_thread([&server]() {
        // Start server
        server.start();
    });

    std::cout << "web server started...\n";


    try
    {
//        activate_rest_service(U("localhost"));
        test_udp_client(argc, argv);
        std::cout << "press enter to exit...\n";
        while (std::cin.get() != '\n')
        {
            continue;
        }
//        close_rest_service();
        exit(0);
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}
