#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define FIFO_CENTRAL_IN "../../central_in"
#define FIFO_CENTRAL_OUT "../../central_out"

void help(const char *program_name) {
    std::cout << "\nUsage: " << program_name << " [command] [arguments]\n\n"
                                                "Available commands:\n\n"
                                                "- add_sensor [sensor_address] [threshold] \n"
                                                "- remove_sensor [sensor_address] \n"
                                                "- set_threshold [sensor_address] \n"
                                                "- get_sensor [sensor_address] \n"
                                                "- add_user [user_name] \n"
                                                "- remove_user [remove_name] \n";

    std::exit(1);
}

void send(std::string msg);

void send(std::string msg) {
    int central_in;
    int central_out;
    char str[BUFSIZ];
    strncpy(str, msg.c_str(), sizeof(str));
    str[sizeof(str) - 1] = 0;

    if ((central_in = open(FIFO_CENTRAL_IN, O_WRONLY)) < 0) {
        throw std::logic_error("can't open fifo: central_in");
    }
    if ((central_out = open(FIFO_CENTRAL_OUT, O_RDONLY)) < 0) {
        throw std::logic_error("can't open fifo: central_out");
    }

    write(central_in, str, sizeof(str));

    perror("Write:");

    read(central_out, str, sizeof(str));

    perror("Read:");
    std::cout << str << std::endl;

    close(central_in);
    close(central_out);
}

int main(int argc, const char *argv[]) {
    try {
        if (argc < 2)
            help(argv[0]);
        else if (std::strcmp(argv[1], "add_sensor") == 0 && argc == 4) {
            std::string msg = std::string(argv[1]) + " " + argv[2] + " " + argv[3];
            send(msg);
        } else if (std::strcmp(argv[1], "remove_sensor") == 0 && argc == 3) {
            std::string msg = std::string(argv[1]) + " " + argv[2];
            send(msg);
        } else if (std::strcmp(argv[1], "set_threshold") == 0 && argc == 4) {
            std::string msg = std::string(argv[1]) + " " + argv[2] + " " + argv[3];
            send(msg);
        } else if (std::strcmp(argv[1], "get_sensor") == 0 && argc == 3) {
            std::string msg = std::string(argv[1]) + " " + argv[2];
            send(msg);
        } else if (std::strcmp(argv[1], "add_user") == 0 && argc == 3) {
            std::string msg = std::string(argv[1]) + " " + argv[2];
            send(msg);
        } else if (std::strcmp(argv[1], "remove_user") == 0 && argc == 3) {
            std::string msg = std::string(argv[1]) + " " + argv[2];
            send(msg);
        } else if (std::strcmp(argv[1], "exit") == 0 && argc == 2) {
            std::string msg = std::string(argv[1]);
            send(msg);
        } else {
            std::cout << argv[1];
            help(argv[0]);
        }
    }
    catch (std::logic_error &error) {
        std::cout << error.what() << std::endl;
    }
    return 0;
}
