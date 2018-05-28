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
                                                "- add_sensor [sensor_address] \n "
                                                "- delete_sensor [sensor_address] \n";
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

    printf("...received from the server: %s\n", str);
    close(central_in);
    close(central_out);
}

int main(int argc, const char *argv[]) {

    printf("Input message to serwer: ");

    if (argc < 2)
        help(argv[0]);
    else if (argc == 4) {
        std::string msg = std::string(argv[1]) + " " + argv[2] + " " + argv[3];

        send(msg);
    } else
        send(argv[1]);

    return 0;
}