#ifndef ROSA_CENTRAL_SENSORLIST_H
#define ROSA_CENTRAL_SENSORLIST_H


#include <mutex>
#include <unordered_map>
#include <vector>

class SensorList {
public:
    enum SensorStatus {CORRECT, NOCOMMUNICATION, NEW};

    struct SensorState{
        SensorState(float current_value, float typical_value, float threshold, SensorStatus status);

        SensorState(float threshold);

        SensorState();

        float current_value = 0;
        float typical_value = 0;
        float threshold = 0;
        SensorStatus status = NEW;
    };

private:
    std::mutex mutex;
    std::unordered_map<std::string, SensorState> sensors;

public:
    void add_sensor(std::string address, float threshold);
    void erase_sensor(std::string address);

    void set_threshold(std::string address, float new_threshold);
    void set_values(std::string address, float new_current_value, float new_typical_value);
    void set_status(std::string address, SensorStatus new_status);
    SensorState get_sensor_state(std::string address);
    std::vector<std::pair<std::string, SensorState>> get_sensors();


};

#endif //ROSA_CENTRAL_SENSORLIST_H
