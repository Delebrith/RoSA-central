#ifndef ROSA_CENTRAL_COMMUNICATOR_H
#define ROSA_CENTRAL_COMMUNICATOR_H

#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "SensorList.h"
#include <boost/algorithm/string.hpp>


class Communicator {
public:
    Communicator(SensorList *sensorList);

    void add_sensor(std::string &address, float threshold);

    void erase_sensor(std::string &address);

    void set_threshold(std::string &address, float new_threshold);

    void ask_for_values(std::string &address);

    SensorList::SensorState get_sensor_state(std::string &address);

    std::vector<std::pair<std::string, SensorList::SensorState>> get_sensor_list();

    void send_server_terminating_msg(std::string port);
private:
    const std::string SensorPort1 = "7000";
    const std::string SensorPort2 = "7001";
    class Callback_set_threshold : public common::UDPClient::Callback {
    public:
        Callback_set_threshold(SensorList *sensorList) : sensorList(sensorList) {}

        virtual void callbackOnReceive(const common::Address &address, std::string msg) {
            try {
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
            catch (std::logic_error &e) {
                std::cout << e.what() << std::endl;
            }
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
                if (answer_splited[0] == "current_value:" && answer_splited[2] == "typical_value:") {
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


    SensorList *sensorList;
    common::UDPClient client;

};


#endif //ROSA_CENTRAL_COMMUNICATOR_H
