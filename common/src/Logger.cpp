//
// Created by pszwed
//

#include <iostream>

#include "terminal_lock.h"
#include "Logger.h"

std::string common::Logger::getTimestamp()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(buffer);
    return str;
}

void common::Logger::log(std::string message)
{
    common::TerminalLock(), std::cout << "[" << getTimestamp() << "] : " << message << std::endl;
}