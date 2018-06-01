#ifndef ROSA_CENTRAL_COMMUNICATOR_H
#define ROSA_CENTRAL_COMMUNICATOR_H

#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "SensorList.h"
#include <boost/algorithm/string.hpp>


class Communicator {
public:
    Communicator();
    //Communicator(common::UDPClient *client, SensorList *sensorList);

    void add_sensor(std::string &address, float threshold);

    void erase_sensor(std::string &address);

    void set_threshold(std::string &address, float new_threshold);

    void ask_for_values(std::string &address, float new_current_value, float new_typical_value);

    SensorList::SensorState get_sensor_state(std::string &address);

    std::vector<std::pair<std::string, SensorList::SensorState>> get_sensor_list();

private:

    class Callback_set_threshold : public common::UDPClient::Callback {
    public:
        Callback_set_threshold(SensorList *sensorList) : sensorList(sensorList) {}

        virtual void callbackOnReceive(const common::Address &address, std::string msg) {
            std::vector<std::string> answer_splited;
            boost::split(answer_splited, msg, [](char c) { return c == ' '; });
            if (answer_splited.size() > 1) {
                if (answer_splited[0] == "threshold:") {
                    float threshold;
                    threshold = std::stof(answer_splited[1]);
                    sensorList->set_threshold(address.hostToString(), threshold);
                    return;
                }
            }
            std::cout << "Bad message from " << address.hostToString() << ": " << msg << std::endl;
            std::cout << "Expected: threshold: <value> " << std::endl;
        }

    private:
        SensorList *sensorList;
    };

    class Callback : public common::UDPClient::Callback {
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

    class Callback_get_value : public common::UDPClient::Callback {
    public:
        Callback_get_value(SensorList *sensorList) : sensorList(sensorList) {}

        virtual void callbackOnReceive(const common::Address &address, std::string msg) {
            std::vector<std::string> answer_splited;
            boost::split(answer_splited, msg, [](char c) { return c == ' '; });
            if (answer_splited.size() > 3) {
                if (answer_splited[0] == "current value:" && answer_splited[2] == "typical value:") {
                    float new_current_value, new_typical_value;
                    new_current_value = std::stof(answer_splited[1]);
                    new_typical_value = std::stof(answer_splited[3]);
                    sensorList->set_values(address.hostToString(), new_current_value, new_typical_value);
                    return;
                }
            }
            std::cout << "Bad message from " << address.hostToString() << ": " << msg << std::endl;
            std::cout << "Expected: current_value: <value> typical_value: <value> " << std::endl;
        }

    private:
        SensorList *sensorList;
    };

    common::UDPClient client;
    SensorList sensorList;

    /*
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
    client.receiveAndCallCallbacks();*/
};


#endif //ROSA_CENTRAL_COMMUNICATOR_H