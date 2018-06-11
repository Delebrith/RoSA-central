//
// Created by M. Swianiewicz, p. szwed, T. Nowak
//


#include <udp_client.h>
#include <udp_server.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "ScriptExecutor.h"
#include "Communicator.h"
#include "SessionList.h"
#include "SensorList.h"
#include "WebServer.h"
#include "exception.h"
#include "Logger.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

SessionList sessionList;
HttpServer httpServer;

void executeScripts(Communicator *communicator) {
    ScriptExecutor executor(communicator);
    try {
        executor.execute();
    }
    catch (std::logic_error &) {
        common::Logger::log(std::string("Problem with creating pipe to listen scripts"));
    }
}

void server(SensorList *sensorList, u_int16_t alarm_server_port, u_int16_t client_port) {

    char buffer[512];
    common::UDPServer server(alarm_server_port);
    while (true) {
        try {
            int retval = server.receive(buffer, 511);
            if (retval < 0) {
                common::Logger::log(std::string("error, no data received"));
                return;
            }

            //ending message
            if (buffer[0] == -1 && server.getClientAddress().isLoopback(client_port)) {
                common::Logger::log(std::string("Alarm server ended"));
                return;
            }

            //alarm
            std::string msg(buffer);
            std::string address;
            std::vector<std::string> message;
            boost::split(message, msg, [](char c) { return c == ' '; });

            address = server.getClientAddress().hostToString();
            if (message.size() == 5 && message[0] == "alarm") {

                if (message[1] == "current_value:" && message[3] == "typical_value:") {
                    float new_current_value, new_typical_value;
                    new_current_value = std::stof(message[2]);
                    new_typical_value = std::stof(message[4]);
                    sensorList->set_values(address, new_current_value, new_typical_value);
                    common::Logger::log(std::string("ALARM!!! Received from " + address + ": " + msg));
                } else {
                    common::Logger::log(std::string("Invalid message from " + address + ": " + msg +
                                                    ". Expected: current_value: <value> typical_value: <value> "));
                }

            } else {
                common::Logger::log(std::string("Invalid message from " + address + ": " + msg +
                                                ". Expected: current_value: <value> typical_value: <value> "));
            }

            server.send(buffer, retval);
            memset(buffer, 0, sizeof(buffer));
        }
        catch (common::ExceptionInfo &) {
            common::Logger::log(std::string("Problem with translating address of alarm sender"));
        }
        catch (std::logic_error &) {
            common::Logger::log(std::string("Address doesn't exist"));
        }
    }
}

void polling(Communicator *communicator, SensorList *sensorList, u_int16_t polling_port, u_int16_t client_port,
             int loop_time) {
    common::UDPServer server(polling_port);
    std::vector<std::string> sensors;
    unsigned int time;
    char buffer;
    while (true) {
        sensors = sensorList->get_addresses();
        if (!sensors.empty()) {
            time = loop_time * 1000 / sensors.size();

            server.getSocket().setReceiveTimeout(time);
            for (auto &it : sensors) {
                communicator->ask_for_values(it);
                if (server.receive(&buffer, 1) >= 0)
                    if (buffer == -1 && server.getClientAddress().isLoopback(client_port)) {
                        common::Logger::log(std::string("Polling thread ended"));
                        return;
                    }
            }
        } else {
            time = loop_time * 1000;
            server.getSocket().setReceiveTimeout(time);
            if (server.receive(&buffer, 1) >= 0)
                if (buffer == -1 && server.getClientAddress().isLoopback(client_port)) {
                    common::Logger::log(std::string("Polling thread ended"));
                    return;
                }
        }
        sensorList->write_to_file();
        common::Logger::log(std::string("Saving list of sensors to file"));
    }

}

void init_from_file(Communicator *communicator) {
    std::ifstream file;
    std::string homeDir = getenv("HOME");

    std::string address;
    float threshold;

    file.open(homeDir + "/.RoSA/data.txt");
    try {
        while (file.peek() != std::ifstream::traits_type::eof()) {
            file >> address >> threshold;
            communicator->add_sensor(address, threshold);
        }
    }
    catch (std::logic_error &error) {
        common::Logger::log(std::string("Adding sensors from file filed, invalid data ") + error.what());

    }
    file.close();
}

int main(int argc, char **argv) {
    constexpr u_int16_t alarm_server_port = 7500;
    constexpr u_int16_t client_port = 7501;
    constexpr u_int16_t polling_port = 7503;
    constexpr u_int16_t sensor_port1 = 7000;
    constexpr u_int16_t sensor_port2 = 7001;
    constexpr int max_answer_time = 5;
    constexpr int loop_time = 40;

    SensorList sensorList(max_answer_time);
    Communicator communicator(&sensorList, client_port, std::to_string(sensor_port1), std::to_string(sensor_port2),
                              max_answer_time);

    if (argc > 1 && std::strcmp(argv[1], "init_from_file") == 0) {
        try {
            init_from_file(&communicator);
        }
        catch (std::logic_error &) {
            common::Logger::log(std::string("Problems with opening file"));
        }
    }

    try {

        WebServer webServer(&sessionList, &communicator, &httpServer);
        thread http_server_thread([]() {
            // Start server
            httpServer.start();
        });

        common::Logger::log(std::string("Http web server started..."));

        thread alarm_server_thread(server, &sensorList, alarm_server_port, client_port);
        common::Logger::log(std::string("Alarm server started..."));

        thread polling_thread(polling, &communicator, &sensorList, polling_port, client_port, loop_time);
        common::Logger::log(std::string("Polling thread started..."));

        executeScripts(&communicator);
        communicator.send_server_terminating_msg(std::to_string(alarm_server_port));
        communicator.send_server_terminating_msg(std::to_string(polling_port));
        httpServer.stop();
        alarm_server_thread.join();
        http_server_thread.join();
        polling_thread.join();
        sensorList.write_to_file();
        common::Logger::log(std::string("Saving list of sensors to file"));
    }
    catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return -1;
    }
}
