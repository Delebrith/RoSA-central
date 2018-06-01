
#include <iostream>
#include <thread>
#include <string>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <sstream>
#include <vector>

#include <udp_server.h>
#include <udp_client.h>

#define SENSOR_PORT1 7000
#define SENSOR_PORT2 7001

#define SERVER_HOST "localhost"
#define SERVER_PORT 7500

#define SERVER2_HOST "localhost"
#define SERVER2_PORT 7501

std::atomic_int threshold(100);
std::atomic_int current_value(40);
std::atomic_int typical(0);


void print(std::string s)
{
    time_t now = time(0);
    std::string dt = ctime(&now);

    dt.erase(std::remove(dt.begin(), dt.end(), '\n'), dt.end());

    std::stringstream sout;
    sout << "<UTC:" << dt << ">" << s << "\n";

    std::cout<<sout.str();
}

int read_current_value()
{
    int a = rand() % 99 + 1;
    return  a;
}

int calculate_median(int * array, uint8_t size)
{
    std::vector<int> medianVec;
    medianVec.assign(array, array+size);
    std::sort(medianVec.begin(), medianVec.end());
    if(size % 2 == 1)
        return medianVec.at((size/2));
    else
        return (medianVec.at((size/2)-1) + medianVec.at((size/2))) / 2;
}

class Callback : public common::UDPClient::Callback
{
public:
    Callback(const std::string str)
            : name(str)
    {}

    virtual void callbackOnReceive(const common::Address &address, std::string msg)
    {
        static std::hash<const common::Address> hasher;
        std::cout << name << " - received: '" << msg << "'\nfrom: ";
        address.print(std::cout);
        std::cout << "(address hash: " << hasher(address) << ")\n";
        std::cout << std::endl;
    }

private:
    std::string name;
};

void updating_values_thread()
{
    int last60[60];
    std::vector<int> medianVec;
    int pointer = 0;
    int amount = 0;
    std::string alarmStr;

    common::Address server_address(SERVER_HOST, SERVER_PORT);
    common::Address server_address2(SERVER_HOST, SERVER_PORT);
    std::unique_ptr<common::UDPClient::Callback> default_callback = std::unique_ptr<common::UDPClient::Callback>(new Callback("default_callback"));
    static common::UDPClient client(6000, 512, std::move(default_callback));

    while(1)
    {
        current_value = read_current_value();
        last60[pointer] = current_value;
        pointer = (pointer + 1) % 60;

        if(amount < 60)
            amount++;
        typical = calculate_median(last60, amount);
        print(" value: " + std::to_string(current_value)+" median "+std::to_string(typical));
        if(current_value > threshold)
        {
            print("value too big. Sending alarm");
            alarmStr = "alarm current_value: "+ std::to_string(current_value) +" typical_value: " + std::to_string(typical);
            try {
                client.sendAndSaveCallback(alarmStr, server_address, std::unique_ptr<common::UDPClient::Callback>(new Callback("callback")));
            }
            catch(const std::exception &ex)
            {
                print(ex.what());
                std::swap(server_address, server_address2);
                client.sendAndSaveCallback(alarmStr, server_address, std::unique_ptr<common::UDPClient::Callback>(new Callback("callback")));
            }

        }
        sleep(2);
    }
}

int set_threshold_receive_thread()
{
    char buffer[17];
    common::UDPServer server(SENSOR_PORT1);
    server.getSocket().setSendTimeout(2000);
    std::string answerStr;
    int retval, tmp;
    while(1)
    {
        try
        {
            memset(buffer, 0, sizeof(buffer));
            retval = server.receive(buffer, 17);
            if(retval < 0)
            {
                print("error receiving data");
                continue;
            }
            print("<-received " + std::string(buffer) + " on port: " + std::to_string(SENSOR_PORT1));

            if(std::string(buffer).find("set_threshold") == 0)
            {
                std::string str = std::string(buffer).substr(std::string("set_threshold").length() + 1, 3);
                if((tmp = std::stoi(str)) > 0)
                {
                    if(tmp < 100)
                    {
                        print("setting new threshold to: " + std::to_string(tmp));
                        threshold = tmp;
                        print( "threshold setted to: " + std::to_string(threshold));

                        answerStr = "threshold: " + std::to_string(tmp);
                        if(server.send(answerStr.c_str(), retval) > 0)
                            print("->sent answer {" + answerStr + "}");

                        else
                            print("failed to sent answer");
                        continue;
                    }
                }
                print("value of threshold not valid: " + std::to_string(tmp));
            }

        }
        catch(const std::exception &ex)
        {
            std::cerr << ex.what() << "\n";
            return -1;
        }

    }
}

int main()
{
    std::thread th1(updating_values_thread);
    std::thread th2(set_threshold_receive_thread);

    srand(time(NULL));
    char buffer[512];
    std::string answerStr;
    common::UDPServer server(SENSOR_PORT2);
    server.getSocket().setSendTimeout(2000);
    int retval;
    print("Sensor started");
    while(1)
    {
        try
        {
            memset(buffer, 0, sizeof(buffer));
            retval = server.receive(buffer, 9);
            if(retval < 0)
            {
                print("error receiving data");
                continue;
            }
            print("<-received " + std::string(buffer) + " on port: " + std::to_string(SENSOR_PORT1));

            if(std::string(buffer).find("get_value") == 0)
            {
                print("server demanded value");
                answerStr = "current_value: "+ std::to_string(current_value) +" typical_value: " + std::to_string(typical);
                if(server.send(answerStr.c_str(), retval) > 0)
                    print("->sent answer {" + answerStr + "}");

                else
                    print("failed to sent answer");
            }

        }
        catch(const std::exception &ex)
        {
            std::cerr << ex.what() << "\n";
            return -1;
        }

    }
}

