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
    int sensor;
    int state;
    u_int32_t timeStamp;

public:
    SensorData(int sensor, int state = SENSOR_BLOCKED, u_int32_t timeStamp = 0)
        : sensor(sensor), state(state), timeStamp(timeStamp) {
    }

    int getState() {
        return state;
    }
    void setState(int s) {
        state = s;
    }
    u_int32_t getTimeStamp() {
        return timeStamp;
    }

    void setTimeStamp(u_int32_t ts) {
        timeStamp = ts;
    }

    char* toString(char* buffer, int size) const {
        snprintf(buffer, size, "sensor: %d state: %d timeStamp: %u", sensor,
                 state, timeStamp);
        return buffer;
    }

    int getSensor() const {
        return sensor;
    }
};

class SensorCapture {
private:
    int sensorsStatus[MAX_SENSOR];
    u_int32_t lastSensortime;

    std::list<SensorData> data;
    int captureState;
    int captureFlags;

    int status;

    void pushSensorData(SensorData sd) {
        if (data.size() >= MAX_SENSORDATA) {
            data.pop_back();
        }
        data.push_front(sd);

        // save last sensor time
        if (sd.getTimeStamp() > lastSensortime)
            lastSensortime = sd.getTimeStamp();
    }

public:
    SensorCapture()
        : captureFlags(CAPTURE_FLAG_NONE),
          lastSensortime(0),
          captureState(CAPTURE_UNKNOWN),
          status(SC_READY) {
        for (int i = 0; i < MAX_SENSOR; ++i)
            sensorsStatus[i] = SENSOR_UNKNOWN;
    }

    void yield(u_int32_t lastSensorTimeToConsider) {
        if (status == SC_READY && lastSensortime <= lastSensorTimeToConsider)
            status = SC_DONE;
    }

    void addSensorData(SensorData sd) {
        sensorsStatus[sd.getSensor()] = sd.getState();

        switch (captureState) {
            // All sensor must be blocked to start capturing
            case CAPTURE_UNKNOWN:
                if (sensorsStatus[SENSOR_1] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_2] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_3] == SENSOR_BLOCKED) {
                    captureState = CAPTURE_READY;
                    captureFlags = CAPTURE_FLAG_NONE;
                    data.clear();
                }
                break;

            // We can only move on if sensor 1 is unblocked (and rest blocked)
            case CAPTURE_READY:
                if (sensorsStatus[SENSOR_1] == SENSOR_UNBLOCKED &&
                    sensorsStatus[SENSOR_2] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_3] == SENSOR_BLOCKED) {
                    captureState = CAPTURE_STARTED;
                    pushSensorData(sd);
                } else
                    captureState = CAPTURE_UNKNOWN;
                break;

            // We stop capture if all sensors are back to closed
            case CAPTURE_STARTED:
                if (sensorsStatus[SENSOR_1] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_2] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_3] == SENSOR_BLOCKED) {
                    captureState = CAPTURE_DONE;
                } else if (sensorsStatus[SENSOR_1] == SENSOR_UNBLOCKED &&
                           sensorsStatus[SENSOR_2] == SENSOR_UNBLOCKED &&
                           sensorsStatus[SENSOR_3] == SENSOR_UNBLOCKED) {
                    captureFlags |= CAPTURE_FLAG_FULLOPEN;
                }
                pushSensorData(sd);
                break;

            // Post capture
            case CAPTURE_DONE:
                if (sensorsStatus[SENSOR_1] == SENSOR_UNBLOCKED &&
                    sensorsStatus[SENSOR_2] == SENSOR_BLOCKED &&
                    sensorsStatus[SENSOR_3] == SENSOR_BLOCKED) {
                    captureFlags |= CAPTURE_FLAG_BOUNCE | CAPTURE_FLAG_SBOUNCE;
                } else if (sensorsStatus[SENSOR_1] == SENSOR_BLOCKED &&
                           sensorsStatus[SENSOR_2] == SENSOR_BLOCKED &&
                           sensorsStatus[SENSOR_3] == SENSOR_UNBLOCKED) {
                    captureFlags |= CAPTURE_FLAG_BOUNCE | CAPTURE_FLAG_EBOUNCE;
                }
                break;
        }
    }

    int getCaptureState() const {
        return captureState;
    }

    int getCaptureFlags() const {
        return captureFlags;
    }

    int getStatus() const {
        return status;
    }

    int getSensorStatus(int sensor) const {
        return sensorsStatus[sensor];
    }

    std::list<SensorData> getData() const {
        return data;
    }
};

class SensorManager {
private:
    std::list<SensorCapture> captures;

public:
    static SensorManager* getInstance() {
        static SensorManager instance;
        return &instance;
    }

    SensorManager() {
        captures.push_front(SensorCapture());
    }

    void add(SensorData data) {
        getCurrentCapture()->addSensorData(data);
    }

    int getCurrentCaptureStatus() {
        return getCurrentCapture()->getStatus();
    }

    int getCurrentSensorStatus(int sensor) {
        return getCurrentCapture()->getSensorStatus(sensor);
    }

    void yield(u_int32_t lastSensorTimeToConsider) {
        getCurrentCapture()->yield(lastSensorTimeToConsider);
        if (getCurrentCapture()->getStatus() == SC_DONE) {
            if (captures.size() >= MAX_CAPTURES) {
                captures.pop_back();
            }
            captures.push_front(SensorCapture());
        }
    }

    std::list<SensorCapture> getCaptures() {
        return captures;
    }

    SensorCapture* getCurrentCapture() {
        return &(captures.front());
    }

    const SensorCapture* getCapture(int n = 0) {
        if (captures.size() > n) {
            if (n == 0)
                return &captures.front();
            // go next
            return &(*std::next(captures.begin(), n));
        }
        return nullptr;
    }
};

#endif