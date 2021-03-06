//
// Created by M. Swianiewicz
//


#include "ScriptExecutor.h"

#define FIFO_IN "../../central_in"
#define FIFO_OUT "../../central_out"

void ScriptExecutor::execute() {
    int central_in;

    int central_out;

    char buf[BUFSIZ];
    std::string return_msg;

    if (mkfifo(FIFO_IN, 0666) && (errno != EEXIST))
        throw std::logic_error("can't create fifo: central_in");
    if (mkfifo(FIFO_OUT, 0666) && (errno != EEXIST)) {
        unlink(FIFO_IN);
        throw std::logic_error("can't create fifo: central_out");
    }


    ssize_t n;
    common::Logger::log(std::string("Listening for scripts - ON."));

    while (true) {
        if ((central_in = open(FIFO_IN, O_RDONLY)) < 0)
            throw std::logic_error("can't open fifo: central_in");
        if ((central_out = open(FIFO_OUT, O_WRONLY)) < 0)
            throw std::logic_error("can't open fifo: central_out");
        n = read(central_in, buf, BUFSIZ);
        if (n == 0) {
            common::Logger::log(std::string("Can't read the script message."));
        }
        if (strcmp("exit", buf) == 0) {
            common::Logger::log(std::string("Listening scripts OFF."));
            break;
        } else if (strcmp("", buf) != 0) {
            std::string msg(buf);
            common::Logger::log(std::string("Script request: " + msg));
            return_msg = execute_command(msg);
            strncpy(buf, return_msg.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = 0;
            n = write(central_out, buf, BUFSIZ);
            if (n == 0) {
                common::Logger::log(std::string("Can't write the script result."));
            }
        }
        memset(buf, 0, sizeof(buf));
        close(central_in);
        close(central_out);
    }

    unlink(FIFO_IN);
    unlink(FIFO_OUT);
}

ScriptExecutor::ScriptExecutor(Communicator *communicator) : communicator(communicator) {}

std::string ScriptExecutor::add_sensor(std::vector<std::string> &command) {
    float threshold;
    try {
        threshold = std::stof(command[2]);
    }
    catch (std::logic_error &) {
        return "Can't add sensor: valid format of threshold";
    }
    try {
        communicator->add_sensor(command[1], threshold);
    }
    catch (std::logic_error &error) {
        return error.what();
    }
    return "Sensor " + command[1] + " added";
}

std::string ScriptExecutor::erase_sensor(std::vector<std::string> &command) {
    try {
        communicator->erase_sensor(command[1]);
    }
    catch (std::logic_error &error) {
        return error.what();
    }
    return "Sensor " + command[1] + " erased";
}

std::string ScriptExecutor::set_threshold(std::vector<std::string> &command) {
    float threshold;
    try {
        threshold = std::stof(command[2]);
    }
    catch (std::logic_error &) {
        return "Can't add sensor: valid format of threshold";
    }
    try {
        communicator->set_threshold(command[1], threshold);
    }
    catch (std::logic_error &error) {
        return error.what();
    }
    return "";
}

std::string ScriptExecutor::get_sensor(std::vector<std::string> &command) {
    try {
        SensorList::SensorState state = communicator->get_sensor_state(command[1]);
        std::string status;
        if (state.status == SensorList::SensorStatus::NOCOMMUNICATION)
            status = "no communication";
        else
            status = "correct";
        return "Sensor: " + command[1] + " current_value: " + std::to_string(state.current_value) +
                " typical_value: " + std::to_string(state.typical_value) +
                " threshold: " + std::to_string(state.threshold) +
                " status: " + status;
    }
    catch (std::logic_error &error) {
        return error.what();
    }
}

std::string ScriptExecutor::ask_sensor(std::vector<std::string> &command) {
    try {
        communicator->ask_for_values(command[1]);
    }
    catch (std::logic_error &error) {
        return error.what();
    }
    return "";
}

std::string ScriptExecutor::execute_command(std::string &msg) {
    std::vector<std::string> command;
    boost::split(command, msg, [](char c) { return c == ' '; });
    if (command[0] == "add_sensor" && command.size() == 3)
        return add_sensor(command);
    if (command[0] == "remove_sensor" && command.size() == 2)
        return erase_sensor(command);
    if (command[0] == "set_threshold" && command.size() == 3)
        return set_threshold(command);
    if (command[0] == "get_sensor" && command.size() == 2)
        return get_sensor(command);
    if (command[0] == "ask_sensor" && command.size() == 2)
        return ask_sensor(command);
    /*if (command[0] == "add_user" && command.size() == 2)
        return add_user(command);
    if (command[0] == "remove_user" && command.size() == 2)
        return erase_user(command);*/
    return "invalid command";
}