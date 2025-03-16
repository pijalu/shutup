#include <unity.h>

#include "SensorManager.cpp"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_singleton(void) {
    SensorManager* sm = SensorManager::getInstance();
    TEST_ASSERT(sm != NULL);
}

void test_addToSensorSeries(void) {
    SensorManager* sm = new SensorManager();
    sm->addToSensorSeries(0, SensorData(true, 1));
    sm->addToSensorSeries(1, SensorData(true, 2));
    sm->addToSensorSeries(2, SensorData(true, 3));

    TEST_ASSERT_EQUAL(sm->getSeries(0).size(), 1);
    TEST_ASSERT_EQUAL(sm->getSeries(1).size(), 1);
    TEST_ASSERT_EQUAL(sm->getSeries(2).size(), 1);

    sm->reset();
    TEST_ASSERT(sm->getSeries(0).empty());
    TEST_ASSERT(sm->getSeries(1).empty());
    TEST_ASSERT(sm->getSeries(2).empty());
}

void test_addManyToSensorSeries(void) {
    SensorManager* sm = new SensorManager();
    for (int i = 0; i < MAX_SENSORDATA + 10; ++i) {
        sm->addToSensorSeries(0, SensorData(i % 2 == 0, i));
        sm->addToSensorSeries(1, SensorData(i % 2 == 0, i));
        sm->addToSensorSeries(2, SensorData(i % 2 == 0, i));
    }
    TEST_ASSERT_EQUAL(MAX_SENSORDATA, sm->getSeries(0).size());
    TEST_ASSERT_EQUAL(MAX_SENSORDATA, sm->getSeries(1).size());
    TEST_ASSERT_EQUAL(MAX_SENSORDATA, sm->getSeries(2).size());
}

void test_addManySameToSensorSeries(void) {
    SensorManager* sm = new SensorManager();
    for (int i = 0; i < MAX_SENSORDATA + 10; ++i) {
        sm->addToSensorSeries(0, SensorData(true, i));
        sm->addToSensorSeries(1, SensorData(false, i));
        sm->addToSensorSeries(2, SensorData(true, i));
    }
    TEST_ASSERT_EQUAL(1, sm->getSeries(0).size());
    TEST_ASSERT_EQUAL(1, sm->getSeries(1).size());
    TEST_ASSERT_EQUAL(1, sm->getSeries(2).size());

    // state should match status and last cnt
    TEST_ASSERT_EQUAL(true, sm->getSeries(0).front().getState());
    TEST_ASSERT_EQUAL(MAX_SENSORDATA + 9,
                      sm->getSeries(0).front().getTimeStamp());

    TEST_ASSERT_EQUAL(false, sm->getSeries(1).front().getState());
    TEST_ASSERT_EQUAL(MAX_SENSORDATA + 9,
                      sm->getSeries(1).front().getTimeStamp());

    TEST_ASSERT_EQUAL(true, sm->getSeries(2).front().getState());
    TEST_ASSERT_EQUAL(MAX_SENSORDATA + 9,
                      sm->getSeries(2).front().getTimeStamp());
}

void test_exceptions(void) {
    SensorManager* sm = new SensorManager();
    // Test out of bounds
    try {
        sm->addToSensorSeries(-1, SensorData(true, 0));
        TEST_FAIL_MESSAGE("Expected std::out_of_range");
    } catch (std::out_of_range& e) {
        // Expected
    } catch (...) {
        TEST_FAIL_MESSAGE(
            "Expected std::out_of_range but got another exception");
    }
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_singleton);
    RUN_TEST(test_addToSensorSeries);
    RUN_TEST(test_addManyToSensorSeries);
    RUN_TEST(test_exceptions);

    UNITY_END();
}
