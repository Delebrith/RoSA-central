#ifndef ROSA_CENTRAL_SENSORLIST_H
#define ROSA_CENTRAL_SENSORLIST_H

#include <mutex>
#include <map>
#include <vector>
#include <address.h>

class SensorList {
public:
    enum SensorStatus {
        CORRECT, NOCOMMUNICATION
    };

    struct SensorState{
        float current_value;
        float typical_value;
        float threshold;
        SensorStatus status;
    };

    struct SensorInfo {
        float current_value = 0;
        float typical_value = 0;
        float threshold = 100;
        bool flag = true;        //true - when question was sent, false - when answer was gotten
        std::time_t time_setting_flag;
        std::time_t last_question;
    };

private:
    int max_answer_time;
    std::mutex mutex;
    std::map<std::string, SensorInfo> sensors;

public:
    SensorList(int max_answer_time);

    void add_sensor(std::string address);
    void erase_sensor(std::string address);

    bool exist(std::string address);
    void set_threshold(std::string address, float new_threshold);
    void set_values(std::string address, float new_current_value, float new_typical_value);

    void set_flag(std::string address);

    SensorState get_sensor_state(std::string address);

    std::vector<std::string> get_addresses();

    void set_last_question(std::string address);
    std::time_t get_last_question(std::string address);

    std::time_t get_time_flag_setting(std::string address);
    std::vector<std::pair<std::string, SensorState>> get_sensors();


};

#endif //ROSA_CENTRAL_SENSORLIST_H
