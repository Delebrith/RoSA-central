//
// Created by delebrith on 27.05.18.
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