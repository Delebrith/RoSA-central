#include "Communicator.h"
#include "exception.h"


void Communicator::add_sensor(std::string &address, float threshold) {
    std::string translated_address;
    try {
        translated_address = common::Address(address, SensorPort1).hostToString();
    }
    catch (common::ExceptionInfo &) {
        throw std::logic_error("Incorrect address format");
    }
    sensorList->add_sensor(translated_address);
    this->set_threshold(address, threshold);
}

void Communicator::erase_sensor(std::string &address) {
    std::string translated_address;
    try {
        translated_address = common::Address(address, SensorPort1).hostToString();
    }
    catch (common::ExceptionInfo &) {
        throw std::logic_error("Incorrect address format");
    }
    sensorList->erase_sensor(translated_address);
}

void Communicator::set_threshold(std::string &address, float new_threshold) {
    if (new_threshold < 0 || new_threshold > 100)
        throw std::invalid_argument("Can't add sensor: valid format of threshold");
    common::Address server_address;
    std::string translated_address;
    try {
        server_address = common::Address(address, SensorPort1);
        translated_address = server_address.hostToString();
    }
    catch (common::ExceptionInfo &) {
        throw std::logic_error("Incorrect address format");
    }
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
    std::cout << "sent to: " << translated_address << std::endl;
    client.sendAndSaveCallback("set_threshold " + std::to_string(new_threshold), server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_set_threshold(sensorList)));
    sensorList->set_last_question(translated_address);
}

void Communicator::ask_for_values(std::string &address) {
    common::Address server_address;
    std::string translated_address;
    try {
        server_address = common::Address(address, SensorPort2);
        translated_address = server_address.hostToString();
    }
    catch (common::ExceptionInfo &) {
        throw std::logic_error("Incorrect address format");
    }
    try {
        std::time_t last_question = sensorList->get_last_question(translated_address);
        if (std::difftime(std::time(nullptr), last_question) < 5) {
            std::cout << "wrong time " << translated_address << std::endl << std::flush;
            return;
        }
    }
    catch (std::logic_error &) {
        std::cout << "don't exist: " << translated_address << std::endl << std::flush;
        throw std::invalid_argument("sensor with address" + translated_address + "doesn't exist");
    }

    std::cout << "sent to: " << translated_address << std::endl << std::flush;
    client.sendAndSaveCallback("get_value", server_address,
                               std::unique_ptr<common::UDPClient::Callback>(new Callback_get_value(sensorList)));
    sensorList->set_last_question(translated_address);
}

SensorList::SensorState Communicator::get_sensor_state(std::string &address) {
    std::string translated_address;
    try {
        translated_address = common::Address(address, SensorPort1).hostToString();
    }
    catch (common::ExceptionInfo &) {
        throw std::logic_error("Incorrect address format");
    }
    return sensorList->get_sensor_state(translated_address);
}

std::vector<std::pair<std::string, SensorList::SensorState>> Communicator::get_sensor_list() {
    return sensorList->get_sensors();
}

Communicator::Communicator(SensorList *sensorList) : sensorList(sensorList), client(7501, 512, std::move(std::unique_ptr
                                                                                                                 <common::UDPClient::Callback>(
        new Callback("default_callback")))) {}
