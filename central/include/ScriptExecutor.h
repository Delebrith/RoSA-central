#ifndef ROSA_CENTRAL_SCRIPTEXECUTOR_H
#define ROSA_CENTRAL_SCRIPTEXECUTOR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "SensorList.h"

class ScriptExecutor {
    SensorList *sensorList;
public:
    ScriptExecutor(SensorList *SensorList);

    void execute();

private:
    std::string add_sensor(std::vector<std::string> &command);

};


#endif //ROSA_CENTRAL_SCRIPTEXECUTOR_H
