#include <udp_client.h>
#include <udp_server.h>
#include <iostream>
#include <cstring>
#include "ScriptExecutor.h"
#include "Communicator.h"

#include "SessionList.h"
#include "SensorList.h"
#include "WebServer.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

SensorList sensorList;
SessionList sessionList;
HttpServer httpServer;


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
    try {
        executor.execute();
    }
    catch (std::logic_error) {
        std::cout << "Problem with creating pipe to listen scripts" << std::endl;
    }
}

void server() {
    try {
        char buffer[512];
        common::UDPServer server(7500);
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
        Communicator communicator(&sensorList);

        WebServer webServer(&sessionList, &communicator, &httpServer);
        std::cout << "web server created...\n";

        thread server_thread([]() {
            // Start server
            httpServer.start();
        });

        std::cout << "web server started...\n";

        executeScripts(&communicator);

        std::cout << "press enter to exit...\n";
        while (std::cin.get() != '\n')
        {
            continue;
        }
        exit(0);

    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}
