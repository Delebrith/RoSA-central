#include <iostream>
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
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
}

void SensorList::set_threshold(std::string address, float new_threshold) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).threshold = new_threshold;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    }
}

void SensorList::set_values(std::string address, float new_current_value, float new_typical_value) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).current_value = new_current_value;
        sensors.at(address).typical_value = new_typical_value;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    }
}

void SensorList::set_status(std::string address, SensorList::SensorStatus new_status) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        sensors.at(address).status = new_status;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    }
}

SensorList::SensorState SensorList::get_sensor_state(std::string address) {
    SensorState state;
    try {
        std::lock_guard<std::mutex> lock(mutex);
        state = sensors.find(address)->second;
    }
    catch (std::out_of_range &){
        throw std::invalid_argument("sensor with address " + address + " doesn't exist");
    }
    return state;
}

std::vector<std::pair<std::string, SensorList::SensorState>> SensorList::get_sensors() {
    std::vector<std::pair<std::string, SensorList::SensorState>> tmp;
    mutex.lock();
    for (auto &it: sensors) {
        tmp.emplace_back(it.first, it.second);
    }
    mutex.unlock();
    return tmp;
}