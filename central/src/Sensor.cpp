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
        case SensorList::SensorStatus::NEW:
            this->status = "NEW";
            break;
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

json::value Sensor::toJSON()
{
    json::value json;
    json[U("address")] = json::value::string(address);
    json[U("currentValue")] = json::value::number(current_value);
    json[U("typicalValue")] = json::value::number(typical_value);
    json[U("threshold")] = json::value::number(threshold);
    json[U("status")] = json::value::string(status);
    return json;
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