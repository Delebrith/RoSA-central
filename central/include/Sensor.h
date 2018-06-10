//
// Created by p.szwed
//

#ifndef ROSA_CENTRAL_SENSOR_H
#define ROSA_CENTRAL_SENSOR_H

#include <string>
#include "SensorList.h"


struct Sensor {
    Sensor(std::string address, SensorList::SensorState state);

    std::string address;
    float current_value = 0;
    float typical_value = 0;
    float threshold = 0;
    std::string status;

    std::string toJSONString();
};

#endif //ROSA_CENTRAL_SENSOR_H
