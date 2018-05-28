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
    if ((central_in = open(FIFO_IN, O_RDONLY)) < 0)
        throw std::logic_error("can't open fifo: central_in");
    if ((central_out = open(FIFO_OUT, O_WRONLY)) < 0)
        throw std::logic_error("can't open fifo: central_out");

    std::cout << "Listening for scripts - ON.\n" << std::endl;

    while (true) //jaki warunek?
    {
        read(central_in, buf, BUFSIZ);
        if (strcmp("exit", buf) == 0) //pomocnicze na teraz
        {
            printf("Listening OFF.\n");
            break;
        } else if (strcmp("", buf) != 0) {
            printf("Received: %s\n", buf);
            std::string msg(buf);
            std::vector<std::string> command;
            boost::split(command, msg, [](char c) { return c == ' '; });
            if (command[0] == "add_sensor" && command.size() == 3) {
                return_msg = add_sensor(command);
            } else if (command[0] == "delete_sensor") {

            } else if (command[0] == "change_thread") {

            }

            strncpy(buf, return_msg.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = 0;
            printf("Sending back...\n");
            write(central_out, buf, BUFSIZ);
        }

        memset(buf, 0, sizeof(buf));
    }
    close(central_in);
    close(central_out);

    unlink(FIFO_IN);
    unlink(FIFO_OUT);
}

ScriptExecutor::ScriptExecutor(SensorList *sensorList) : sensorList(sensorList) {}

std::string ScriptExecutor::add_sensor(std::vector<std::string> &command) {
    float number;
    try {
        number = std::stof(command[2]);
    }
    catch (std::logic_error &) {
        return "Can't add sensor: valid format of threshold";
    }
    try {
        sensorList->add_sensor(command[1], number);
    }
    catch (std::logic_error &error) {
        return error.what();
    }
    return "Sensor " + command[1] + " added";
}