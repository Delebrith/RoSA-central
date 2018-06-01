#include "Communicator.h"



void Communicator::add_sensor(std::string &address, float threshold) {
    std::string translated_address = common::Address(address, "7000").toString();
    std::cout << "address: " << translated_address << std::endl;
    sensorList->add_sensor(translated_address);
    this->set_threshold(address, threshold);
}

void Communicator::erase_sensor(std::string &address) {
    std::string translated_address = common::Address(address, "7000").toString();
    sensorList->erase_sensor(translated_address);
}

void Communicator::set_threshold(std::string &address, float new_threshold) {
    if (new_threshold < 0 || new_threshold > 100)
        throw std::invalid_argument("Can't add sensor: valid format of threshold");
    common::Address server_address(address, "7000");
    std::string translated_address = server_address.toString();
    std::cout << "address: " << translated_address << std::endl;
    try {
        std::time_t last_question = sensorList->get_last_question(translated_address);
        if (std::difftime(std::time(nullptr), last_question) < 5) {
            std::cout << "wrong time " << translated_address << std::endl;
            return;
        }
    }
    catch (std::logic_error &) {
        std::cout << "don't exist: " << translated_address << std::endl;
        throw std::invalid_argument("sensor with address" + translated_address + "doesn't exist");
    }
    std::cout << "sent: " << translated_address << std::endl;
    client.sendAndSaveCallback("set_threshold " + std::to_string(new_threshold), server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_set_threshold(sensorList)));
}

void Communicator::ask_for_values(std::string &address, float new_current_value, float new_typical_value) {
    common::Address server_address(address, "7000");
    std::string translated_address = server_address.toString();
    try {
        std::time_t last_question = sensorList->get_last_question(translated_address);
        if (std::difftime(last_question, std::time(nullptr)) < 5)
            return;
    }
    catch (std::logic_error &) {
        throw std::invalid_argument("sensor with address" + translated_address + "doesn't exist");
    }

    client.sendAndSaveCallback("get_value", server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_get_value(sensorList)));
}

SensorList::SensorState Communicator::get_sensor_state(std::string &address) {
    std::string translated_address = common::Address(address, "7000").toString();
    return sensorList->get_sensor_state(translated_address);
}

std::vector<std::pair<std::string, SensorList::SensorState>> Communicator::get_sensor_list() {
    return sensorList->get_sensors();
}

Communicator::Communicator(SensorList *sensorList) : sensorList(sensorList), client(6000, 512, std::move(std::unique_ptr
                                                                                                                 <common::UDPClient::Callback>(
        new Callback("default_callback")))) {}
