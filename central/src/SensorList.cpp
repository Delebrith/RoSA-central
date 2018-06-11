//
// Created by M. Swianiewicz
//


#include "SensorList.h"
#include "Logger.h"
#include <fstream>

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
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
}

void SensorList::set_threshold(std::string address, float new_threshold) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    iterator->second.threshold = new_threshold;
    iterator->second.flag = false;
}

void SensorList::set_values(std::string address, float new_current_value, float new_typical_value) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    iterator->second.current_value = new_current_value;
    iterator->second.typical_value = new_typical_value;
    iterator->second.flag = false;
}

SensorList::SensorState SensorList::get_sensor_state(std::string address) {
    SensorState state;
    SensorInfo info;

    {
        std::lock_guard<std::mutex> lock(mutex);
        auto iterator = sensors.find(address);
        if (iterator == sensors.end())
            throw std::invalid_argument("sensor with address " + address + " doesn't exist");
        info = iterator->second;
    }
    state.threshold = info.threshold;
    state.current_value = info.current_value;
    state.typical_value = info.typical_value;
    if (!info.flag || std::difftime(std::time(nullptr), info.time_setting_flag) < max_answer_time)
        state.status = CORRECT;
    else
        state.status = NOCOMMUNICATION;
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
        if (!it.second.flag || std::difftime(std::time(nullptr), it.second.time_setting_flag) < max_answer_time)
            state.status = CORRECT;
        else
            state.status = NOCOMMUNICATION;
        tmp.emplace_back(it.first, state);
    }
    return tmp;
}

std::vector<std::string> SensorList::get_addresses() {
    std::vector<std::string> tmp;
    std::lock_guard<std::mutex> lock(mutex);
    for (auto &it: sensors) {
        tmp.push_back(it.first);
    }
    return tmp;
}

std::time_t SensorList::get_time_flag_setting(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    return iterator->second.time_setting_flag;
}

void SensorList::set_last_value_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    iterator->second.last_value_question = std::time(nullptr);
    if (!iterator->second.flag) {
        iterator->second.flag = true;
        iterator->second.time_setting_flag = std::time(nullptr);
    } else if (std::difftime(std::time(nullptr), iterator->second.time_setting_flag) > max_answer_time) {
        common::Logger::log(std::string("WARNING! Sensor " + iterator->first + " doesn't respond"));
    }
}

std::time_t SensorList::get_last_value_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    return iterator->second.last_value_question;
}

void SensorList::set_last_threshold_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    iterator->second.last_threshold_question = std::time(nullptr);
    if (!iterator->second.flag) {
        iterator->second.flag = true;
        iterator->second.time_setting_flag = std::time(nullptr);
    } else if (std::difftime(std::time(nullptr), iterator->second.time_setting_flag) > max_answer_time) {
        common::Logger::log(std::string("WARNING! Sensor " + iterator->first + " doesn't respond"));
    }
}

std::time_t SensorList::get_last_threshold_question(std::string address) {
    std::lock_guard<std::mutex> lock(mutex);
    auto iterator = sensors.find(address);
    if (iterator == sensors.end())
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    return iterator->second.last_threshold_question;
}


SensorList::SensorList(int max_answer_time) : max_answer_time(max_answer_time) {}


void SensorList::write_to_file() {
    std::vector<std::pair<std::string, float>> addresses;
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto &it: sensors) {
            addresses.emplace_back(it.first, it.second.threshold);
        }
    }

    std::ofstream file;
    std::string homeDir = getenv("HOME");

    file.open(homeDir + "/.RoSA/data.txt", std::fstream::out);
    for (auto &it: addresses) {
        file << it.first << " " << it.second << std::endl;
    }
    file.close();
}
