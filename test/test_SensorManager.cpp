#include <unity.h>
#include "SensorManager.cpp"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
    SensorManager* sm  = SensorManager::getInstance();
    TEST_ASSERT(sm != NULL);

    sm->addToSensorSeries(0, SensorData(true, 1));
    sm->addToSensorSeries(1, SensorData(true, 2));
    sm->addToSensorSeries(2, SensorData(true, 3));

    TEST_ASSERT(sm->getSeries(0).size() == 1);
    TEST_ASSERT(sm->getSeries(1).size() == 1);
    TEST_ASSERT(sm->getSeries(2).size() == 1);
  
    sm->reset();
    TEST_ASSERT(sm->getSeries(0).empty());
    TEST_ASSERT(sm->getSeries(1).empty());
    TEST_ASSERT(sm->getSeries(2).empty());

    for (int i = 0; i < MAX_SENSORDATA + 10; ++i) {
        sm->addToSensorSeries(0, SensorData(true, i));
        sm->addToSensorSeries(1, SensorData(false, i));
        sm->addToSensorSeries(2, SensorData(true, i));
    }
    TEST_ASSERT(sm->getSeries(0).size() == MAX_SENSORDATA);
    TEST_ASSERT(sm->getSeries(1).size() == MAX_SENSORDATA);
    TEST_ASSERT(sm->getSeries(2).size() == MAX_SENSORDATA);

    // Test out of bounds
    try {
        sm->addToSensorSeries(-1, SensorData(true, 0));
        TEST_FAIL_MESSAGE("Expected std::out_of_range");
    } catch (std::out_of_range& e) {
        // Expected
    } catch (...) {
        TEST_FAIL_MESSAGE("Expected std::out_of_range but got another exception");
    }

    UNITY_END();
}
