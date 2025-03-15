#include "SensorManager.h"

SensorData::SensorData(bool state, u_int32_t timeStamp) 
    :state(state), timeStamp(timeStamp) {
}

bool SensorData::getState() { 
    return state; 
}

u_int32_t SensorData::getTimeStamp()  { 
    return timeStamp; 
}  

char* SensorData::toString(char* buffer) { 
    sprintf(buffer, "State: %d, TimeStamp: %u", state, timeStamp);
    return buffer;
}

SensorSeries::SensorSeries() {
    // Initialize the SensorSeries with an empty list of SensorData
}

void SensorSeries::add(SensorData data) {
    if (sensorDataList.size() >= MAX_SENSORDATA) {
        sensorDataList.pop_back();
    }
    sensorDataList.push_front(data);
}

void SensorSeries::clear() {
    sensorDataList.clear();
}

std::list<SensorData> SensorSeries::getSeries() {
    return sensorDataList;
}

SensorManager::SensorManager() {
    // Initialize the SensorManager with an empty list of SensorSeries
}   

SensorManager *SensorManager::getInstance() {
    static SensorManager instance;
    return &instance;
}

void SensorManager::addToSensorSeries(int series, SensorData data) {
    if (series >= MAX_SENSOR || series < 0) {
        throw std::out_of_range("Invalid sensor series index");
        return;
    }
    seriesList[series].add(data);
}

void SensorManager::reset() {
    for (int i = 0; i < MAX_SENSOR; ++i) {
        seriesList[i].clear();
    }
}

std::list<SensorData> SensorManager::getSeries(int series) {
    if (series >= MAX_SENSOR || series < 0) {
        throw std::out_of_range("Invalid sensor series index");
    }   
    return seriesList[series].getSeries();
}