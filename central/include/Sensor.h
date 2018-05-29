//
// Created by delebrith on 27.05.18.
//

#ifndef ROSA_CENTRAL_SENSOR_H
#define ROSA_CENTRAL_SENSOR_H

#include <string>
#include <cpprest/json.h>
#include "SensorList.h"

using namespace web;
using namespace utility;

struct Sensor {
    Sensor(std::string address, SensorList::SensorState state);

    std::string address;
    float current_value = 0;
    float typical_value = 0;
    float threshold = 0;
    std::string status;

    json::value toJSON();
};

#endif //ROSA_CENTRAL_SENSOR_H
