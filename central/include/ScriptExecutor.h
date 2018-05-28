#ifndef ROSA_CENTRAL_SCRIPTEXECUTOR_H
#define ROSA_CENTRAL_SCRIPTEXECUTOR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "SensorList.h"
#include "UserList.h"

class ScriptExecutor {
    SensorList *sensorList;
    UserList *userList;

public:
    ScriptExecutor(SensorList *SensorList, UserList *userList);

    void execute();

private:
    std::string execute_command(std::string &msg);

    std::string add_sensor(std::vector<std::string> &command);

    std::string erase_sensor(std::vector<std::string> &command);

    std::string set_threshold(std::vector<std::string> &command);

    std::string get_sensor(std::vector<std::string> &command);

    std::string add_user(std::vector<std::string> &command);

    std::string erase_user(std::vector<std::string> &command);

};


#endif //ROSA_CENTRAL_SCRIPTEXECUTOR_H
