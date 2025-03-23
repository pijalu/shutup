#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <iterator>
#include <list>
#include <stdexcept>
#include <vector>

#include "defs.h"

// Store a state and associated timestamp for a sensor
class SensorData {
private:
    int state;
    u_int32_t timeStamp;

public:
    SensorData(int state = SENSOR_BLOCKED, u_int32_t timeStamp = 0);

    int getState();
    void setState(int s);

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

public:
    static SensorManager* getInstance();

    SensorManager();

    // reset all states
    void reset();
    void addToSensorSeries(int series, SensorData data);
    int getCurrentStatus(int series);
    std::list<SensorData> getSeries(int series);
};

#endif