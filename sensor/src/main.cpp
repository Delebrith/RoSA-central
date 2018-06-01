#include <iostream>
#include <thread>
#include <string>
#include <time.h>
#include <unistd.h>
#include <vector>

#include <udp_server.h>

#define SENSOR_PORT 7000

int thres_hold = 0;
int current_value = 40;
int typical = 0;

int read_current_value() {
    int a = rand() % 99 + 1;
    return a;
}

void updating_values_thread() {
    std::cout << "entering thread" << "\n";
    int last60[60];
    std::vector<int> medianVec;
    int pointer = 0;
    int amount = 0;
    while (1)
    {
        pointer = (pointer + 1) % 60;
        current_value = read_current_value();
        std::cout << "value: " << current_value << "\n";
        last60[pointer] = current_value;
        if (amount < 60)
        {

        }
        sleep(60);
    }
}

int main() {
    std::thread th(updating_values_thread);

    srand(time(NULL));
    char buffer[512];
    int tmp;
    std::string answerStr;
    common::UDPServer server(SENSOR_PORT);
    server.getSocket().setSendTimeout(2000);
    int retval;
    std::cout << "Sensor started\n";
    while (1) {
        try {
            memset(buffer, 0, sizeof(buffer));
            retval = server.receive(buffer, 511);
            if (retval < 0) {
                std::cout << "error receiving data\n";
                return -1;
            }
            std::cout << "<-received" << buffer << "\n";

            if (std::string(buffer).find("set_threshold") == 0) {
                std::string str = std::string(buffer).substr(std::string("set_threshold").length() + 1, 3);
                if ((tmp = std::stoi(str)) > 0) {
                    if (tmp < 100) {
                        std::cout << "setting new threshold to: " << tmp << "\n";
                        thres_hold = tmp;
                        std::cout << "threshold setted to: " << thres_hold << "\n";

                        answerStr = "threshold: " + std::to_string(tmp);
                        if (server.send(answerStr.c_str(), answerStr.size()) > 0)
                            std::cout << "->sent answer {" << answerStr << "}\n";
                        else
                            std::cout << "failed to sent answer\n";
                        continue;
                    }
                }
                std::cout << "value of threshold not valid: " << tmp << "\n";
            } else if (std::string(buffer).find("get_value") == 0) {
                std::cout << "server demanded value" << "\n";
                answerStr =
                        "current_value: " + std::to_string(current_value) + " typical_value: " +
                        std::to_string(typical);
                if (server.send(answerStr.c_str(), answerStr.size()) > 0)
                    std::cout << "->sent answer {" << answerStr << "}\n";
                else
                    std::cout << "failed to sent answer\n";
            }

        }
        catch (const std::exception &ex) {
            std::cerr << ex.what() << "\n";
            return -1;
        }

    }
    th.join();
    return 0;
}


