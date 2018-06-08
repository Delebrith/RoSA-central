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
    if (threshold < 0 || threshold > 100)
        throw std::invalid_argument("Can't add sensor: valid format of threshold");
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
        if (std::difftime(std::time(nullptr), last_question) < 5)
            return;
    }
    catch (std::logic_error &) {
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
        if (std::difftime(std::time(nullptr), last_question) < 5)
            return;
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

void Communicator::send_server_terminating_msg(std::string port) {
    char magic = -1;
    if (client.send(&magic, 1, common::Address("localhost", port)) < 0) {
        common::ExceptionInfo::warning(
                "CRITICAL ERROR - could not send kill message to receiver thread - the program might lock down");
    }
}


Communicator::Callback_set_threshold::Callback_set_threshold(SensorList *sensorList) : sensorList(sensorList) {}

void Communicator::Callback_set_threshold::callbackOnReceive(const common::Address &address, std::string msg) {
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
        common::Logger::log(std::string("Invalid answer from " + address.hostToString() + ": " + msg + "\n" +
                                        "Expected: threshold: <value> "));
    }
    catch (std::logic_error &e) {
        std::cout << e.what() << std::endl;
    }
}

Communicator::Callback_get_value::Callback_get_value(SensorList *sensorList) : sensorList(sensorList) {}

void Communicator::Callback_get_value::callbackOnReceive(const common::Address &address, std::string msg) {
    try {
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
        common::Logger::log(std::string("Invalid answer from " + address.hostToString() + ": " + msg + "\n" +
                                        "Expected: current_value: <value> typical_value: <value> "));

    }
    catch (std::logic_error &e) {
        std::cout << e.what() << std::endl;
    }
}

Communicator::Callback::Callback(const std::string str) : name(str) {}

void Communicator::Callback::callbackOnReceive(const common::Address &address, std::string msg) {
    common::Logger::log(std::string("Unexpected callback, answer from" + address.hostToString() + ": " + msg));
}