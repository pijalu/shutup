#include "SensorManager.h"

SensorData::SensorData(int state, u_int32_t timeStamp)
    : state(state), timeStamp(timeStamp) {
}

int SensorData::getState() {
    return state;
}

void SensorData::setState(int s) {
    state = s;
}

void SensorData::setTimeStamp(u_int32_t ts) {
    timeStamp = ts;
}

u_int32_t SensorData::getTimeStamp() {
    return timeStamp;
}

char* SensorData::toString(char* buffer, int size) {
    snprintf(buffer, size, "State: %d, TimeStamp: %u", state, timeStamp);
    return buffer;
}

SensorSeries::SensorSeries() {
    // Initialize the SensorSeries with an empty list of SensorData
}

void SensorSeries::add(SensorData data) {
    if (sensorDataList.size() >= MAX_SENSORDATA) {
        sensorDataList.pop_back();
    }
    SensorData currentFront = sensorDataList.front();
    if (currentFront.getState() == data.getState()) {
        currentFront.setTimeStamp(data.getTimeStamp());
    } else {
        sensorDataList.push_front(data);
    }
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

SensorManager* SensorManager::getInstance() {
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

int SensorManager::getCurrentStatus(int series) {
    if (series >= MAX_SENSOR || series < 0) {
        throw std::out_of_range("Invalid sensor series index");
    }
    std::list<SensorData> seriesData = seriesList[series].getSeries();
    if (!seriesData.empty()) {
        return seriesData.front().getState();
    }
    return false;
}