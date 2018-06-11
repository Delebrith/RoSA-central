//
// Created by p.szwed
//

#include "Sensor.h"

Sensor::Sensor(std::string address, SensorList::SensorState state)
{
    this->address = address;
    this->current_value = state.current_value;
    this->typical_value = state.typical_value;
    this->threshold = state.threshold;

    switch (state.status)
    {
        case SensorList::SensorStatus::CORRECT:
            this->status = "CORRECT";
            break;
        case SensorList::SensorStatus::NOCOMMUNICATION:
            this->status = "NOCOMMUNICATION";
            break;
        default:
            this->status = "[UNKNOWN]";
    }
}


std::string Sensor::toJSONString()
{
    std::string json = "{\n";
    json += "\"address\": \"";
    json += this->address;
    json += "\",\n";
    json += "\"status\": \"";
    json += this->status;
    json += "\",\n";
    json += "\"threshold\": ";
    json += std::to_string(this->threshold);
    json += ",\n";
    json += "\"currentValue\": ";
    json += std::to_string(this->current_value);
    json += ",\n";
    json += "\"typicalValue\": ";
    json += std::to_string(this->typical_value);
    json += "\n}";
    return json;
}