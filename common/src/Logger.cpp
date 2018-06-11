//
// Created by p.szwed
//

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "file_lock.h"
#include "terminal_lock.h"
#include "Logger.h"

std::string common::Logger::getTimestamp() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
}

void common::Logger::log(std::string message) {
    std::string timestamp = getTimestamp();
    common::TerminalLock(), std::cout << "[" << timestamp << "] : " << message << std::endl;
    std::ofstream file;
    std::string homeDir = getenv("HOME");
    common::FileLock();
    file.open(homeDir + "/.RoSA/logs.txt", std::fstream::out | std::fstream::app);
    file << "[" << timestamp << "] : " << message << std::endl;
    file.flush();
    file.close();

}