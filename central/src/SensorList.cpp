#include "SensorList.h"

SensorList::SensorState::SensorState(float current_value, float typical_value, float threshold,
                                     SensorList::SensorStatus status) : current_value(current_value),
                                                                        typical_value(typical_value),
                                                                        threshold(threshold), status(status) {}

SensorList::SensorState::SensorState(float threshold) : threshold(threshold) {}

SensorList::SensorState::SensorState() {}

void SensorList::add_sensor(std::string address, float threshold) {
    std::lock_guard<std::mutex> lock(mutex);
    sensors.emplace(address, threshold);
}

void SensorList::erase_sensor(std::string address) {
    int i;
    {
        std::lock_guard<std::mutex> lock(mutex);
        i = sensors.erase(address);
    }
    if(i == 0)
        throw std::invalid_argument("sensor with address" + address +"doesn't exist");
}

void SensorList::set_threshold(std::string address, float new_threshold) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).threshold = new_threshold;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address" + address +"doesn't exist");
    }
}

void SensorList::set_values(std::string address, float new_current_value, float new_typical_value) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).current_value = new_current_value;
        sensors.at(address).typical_value = new_typical_value;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address" + address +"doesn't exist");
    }
}

SensorList::SensorState SensorList::get_sensor_state(std::string address) {
    SensorState state;
    SensorInfo info;
    try {
        std::lock_guard<std::mutex> lock(mutex);
        info = sensors.find(address)->second;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address" + address +"doesn't exist");
    }
    state.threshold = info.threshold;
    state.current_value = info.current_value;
    state.typical_value = info.typical_value;
    if (std::difftime(info.last_answer, info.last_question) > 0 ||
        std::difftime(info.last_question, std::time(nullptr)) < 5)
        state.status = NOCOMMUNICATION;
    else
        state.status = CORRECT;
    return state;
}

std::vector<std::pair<std::string, SensorList::SensorState>> SensorList::get_sensors() {
    std::vector<std::pair<std::string, SensorList::SensorState>> tmp;
    mutex.lock();
    for (auto &it: sensors) {
        SensorState state;
        state.threshold = it.second.threshold;
        state.current_value = it.second.current_value;
        state.typical_value = it.second.typical_value;
        if (std::difftime(it.second.last_answer, it.second.last_question) > 0 ||
            std::difftime(it.second.last_question, std::time(nullptr)) < 5)
            state.status = CORRECT;
        else
            state.status = NOCOMMUNICATION;
        tmp.emplace_back(it.first, state);
    }
    mutex.unlock();
    return tmp;
}

void SensorList::set_last_question(std::string address, std::time_t last_question) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).last_question = last_question;
    }
    catch (std::out_of_range &) {
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    }
}

void SensorList::set_last_answer(std::string address, std::time_t last_answer) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).last_answer = last_answer;
    }
    catch (std::out_of_range &) {
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    }
}
