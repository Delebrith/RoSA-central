#include "Communicator.h"

Communicator::Communicator() : client(6000, 512, std::move(std::unique_ptr
                                                                   <common::UDPClient::Callback>(
        new Callback("default_callback")))) {}

void Communicator::add_sensor(std::string &address, float threshold) {
    sensorList.add_sensor(address);
    this->set_threshold(address, threshold);
}

void Communicator::erase_sensor(std::string &address) {
    sensorList.erase_sensor(address);
}

void Communicator::set_threshold(std::string &address, float new_threshold) {
    if (new_threshold < 0 || new_threshold > 100)
        throw std::invalid_argument("Can't add sensor: valid format of threshold");
    try {
        std::time_t last_question = sensorList.get_last_question(address);
        if (std::difftime(last_question, std::time(nullptr)) < 5)
            return;
    }
    catch (std::logic_error &) {
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    }
    common::Address server_address(address, "7000");
    client.sendAndSaveCallback("set_threshold " + std::to_string(new_threshold), server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_set_threshold(&sensorList)));
}

void Communicator::ask_for_values(std::string &address, float new_current_value, float new_typical_value) {
    try {
        std::time_t last_question = sensorList.get_last_question(address);
        if (std::difftime(last_question, std::time(nullptr)) < 5)
            return;
    }
    catch (std::logic_error &) {
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    }
    common::Address server_address(address, "7001");
    client.sendAndSaveCallback("get_value", server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_get_value(&sensorList)));
}

SensorList::SensorState Communicator::get_sensor_state(std::string &address) {
    return sensorList.get_sensor_state(address);
}

std::vector<std::pair<std::string, SensorList::SensorState>> Communicator::get_sensor_list() {
    return sensorList.get_sensors();
}
