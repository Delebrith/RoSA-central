//
// Created by M. Swianiewicz
//


#ifndef ROSA_CENTRAL_COMMUNICATOR_H
#define ROSA_CENTRAL_COMMUNICATOR_H

#include <udp_client.h>
#include <iostream>
#include <cstring>
#include "SensorList.h"
#include <boost/algorithm/string.hpp>
#include "Logger.h"

class Communicator {
public:
    Communicator(SensorList *sensorList, int client_port, std::string sensor_port1, std::string sensor_port2,
                 int max_answer_time);

    void add_sensor(std::string &address, float threshold);

    void erase_sensor(std::string &address);

    void set_threshold(std::string &address, float new_threshold);

    void ask_for_values(std::string &address);

    SensorList::SensorState get_sensor_state(std::string &address);

    std::vector<std::pair<std::string, SensorList::SensorState>> get_sensor_list();

    void send_server_terminating_msg(std::string port);

private:


    class Callback_set_threshold : public common::UDPClient::Callback {
    public:
        Callback_set_threshold(SensorList *sensorList);

        void callbackOnReceive(const common::Address &address, std::string msg) override;

    private:
        SensorList *sensorList;
    };

    class Callback : public common::UDPClient::Callback {
    public:
        Callback(const std::string str);

        void callbackOnReceive(const common::Address &address, std::string msg) override;

    private:
        std::string name;
    };

    class Callback_get_value : public common::UDPClient::Callback {
    public:
        Callback_get_value(SensorList *sensorList);

        void callbackOnReceive(const common::Address &address, std::string msg) override;

    private:
        SensorList *sensorList;
    };

    SensorList *sensorList;
    common::UDPClient client;
    std::string sensor_port1;
    std::string sensor_port2;
    int max_answer_time;

};


#endif //ROSA_CENTRAL_COMMUNICATOR_H
