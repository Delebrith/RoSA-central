#include <udp_client.h>
#include <udp_server.h>
#include <iostream>
#include <cstring>
#include "ScriptExecutor.h"
#include "Communicator.h"
#include "Logger.h"
#include "SessionList.h"
#include "SensorList.h"
#include "WebServer.h"
#include "exception.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

SensorList sensorList;
SessionList sessionList;
HttpServer httpServer;

void executeScripts(Communicator *communicator) {
    ScriptExecutor executor(communicator);
    try {
        executor.execute();
    }
    catch (std::logic_error &) {
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

            //ending message
            if (buffer[0] == -1 && server.getClientAddress().isLoopback(7501)) {
                std::cout << "server ending work\n";
                return;
            }

            //alarm
            std::string msg(buffer);
            std::string address;
            std::vector<std::string> message;
            boost::split(message, msg, [](char c) { return c == ' '; });
            try {
                address = server.getClientAddress().hostToString();
                std::cout << "Received from " << address << ": " << msg << std::endl;
                if (message.size() == 5 && message[0] == "alarm") {

                    if (message[1] == "current_value:" && message[3] == "typical_value:") {
                        float new_current_value, new_typical_value;
                        new_current_value = std::stof(message[2]);
                        new_typical_value = std::stof(message[4]);
                        sensorList.set_values(address, new_current_value, new_typical_value);
                    } else {
                        std::cout << "Bad message from " << address << ": " << msg << std::endl;
                        std::cout << "Expected: current_value: <value> typical_value: <value> " << std::endl;
                    }

                } else {
                    std::cout << "Bad message from " << address << ": " << msg << std::endl;
                    std::cout << "Expected: current_value: <value> typical_value: <value> " << std::endl;
                }
            }
            catch (common::ExceptionInfo &) {
                std::cout << "Problem with translating address of alarm sender" << std::endl;
            }

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



int main(int, char **)
{

    try
    {
        Communicator communicator(&sensorList);

        WebServer webServer(&sessionList, &communicator, &httpServer);

        thread http_server_thread([]() {
            httpServer.start();
        });
        common::Logger::log(std::string("Http web server started..."));

        thread alarm_server_thread(server);
        common::Logger::log(std::string("Alarm server started..."));

        executeScripts(&communicator);
        communicator.send_server_terminating_msg();
        httpServer.stop();
        alarm_server_thread.join();
        http_server_thread.join();
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}
