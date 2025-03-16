#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <iterator>
#include <list>
#include <stdexcept>
#include <vector>

#define MAX_SENSOR 3
#define MAX_SENSORDATA 1000

// Store a state and associated timestamp for a sensor
class SensorData {
private:
    bool state;
    u_int32_t timeStamp;

public:
    SensorData(bool state = false, u_int32_t timeStamp = 0);

    bool getState();
    void setState(bool s);

    void setTimeStamp(u_int32_t ts);
    u_int32_t getTimeStamp();

    char* toString(char* buffer, int size);
};

class SensorSeries {
public:
    SensorSeries();
    void add(SensorData data);
    void clear();
    std::list<SensorData> getSeries();

private:
    std::list<SensorData> sensorDataList;
};

class SensorManager {
private:
    SensorSeries seriesList[MAX_SENSOR];
    SensorManager();

public:
    static SensorManager* getInstance();

    // reset all states
    void reset();
    void addToSensorSeries(int series, SensorData data);
    std::list<SensorData> getSeries(int series);
};

#endif