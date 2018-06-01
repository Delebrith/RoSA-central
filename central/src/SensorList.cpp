#include "SensorList.h"

void SensorList::add_sensor(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    sensors.emplace(address, SensorInfo());
}

void SensorList::erase_sensor(std::string address) {
    int i;
    {
        std::lock_guard<std::mutex> lock(mutex);
        i = sensors.erase(address);
    }
    if (i == 0)
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
}

void SensorList::set_threshold(std::string address, float new_threshold) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    iterator->second.threshold = new_threshold;
    iterator->second.last_answer = std::time(nullptr);
}

void SensorList::set_values(std::string address, float new_current_value, float new_typical_value) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    iterator->second.current_value = new_current_value;
    iterator->second.typical_value = new_typical_value;
    iterator->second.last_answer = std::time(nullptr);
}

SensorList::SensorState SensorList::get_sensor_state(std::string address) {
    SensorState state;
    SensorInfo info;

    {
        std::lock_guard<std::mutex> lock(mutex);
        auto iterator = sensors.find(address);
        if (iterator == sensors.end())
            throw std::invalid_argument("sensor with address" + address + "doesn't exist");
        info = iterator->second;
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
    std::lock_guard<std::mutex> lock(mutex);
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
    return tmp;
}

void SensorList::set_last_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    iterator->second.last_question = std::time(nullptr);
}

void SensorList::set_last_answer(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    iterator->second.last_answer = std::time(nullptr);
}

bool SensorList::exist(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    return iterator != sensors.end();
}

std::time_t SensorList::get_last_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address" + address + "doesn't exist");
    return iterator->second.last_question;
}
