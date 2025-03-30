#include <unity.h>

#include "SensorManager.h"

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

void test_sensorManagerState(void) {
    SensorManager* sm = new SensorManager();
    // Capture should be ready
    TEST_ASSERT_EQUAL(SC_READY, sm->getCurrentCaptureStatus());
    // Status should be unknown
    TEST_ASSERT_EQUAL(SENSOR_UNKNOWN, sm->getCurrentSensorStatus(SENSOR_1));
    TEST_ASSERT_EQUAL(SENSOR_UNKNOWN, sm->getCurrentSensorStatus(SENSOR_2));
    TEST_ASSERT_EQUAL(SENSOR_UNKNOWN, sm->getCurrentSensorStatus(SENSOR_2));
    // Check currect Capture
    TEST_ASSERT_EQUAL(CAPTURE_UNKNOWN,
                      sm->getCurrentCapture()->getCaptureState());
    TEST_ASSERT_EQUAL(CAPTURE_FLAG_NONE,
                      sm->getCurrentCapture()->getCaptureFlags());
}

void test_addSensorDataAndStateChange(void) {
    SensorManager* sm = new SensorManager();
    int ts = 0;

    // add 3 sensor as blocked - switch to ready only when 3 are blocked
    sm->add(SensorData(SENSOR_1, SENSOR_BLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_UNKNOWN,
                      sm->getCurrentCapture()->getCaptureState());

    sm->add(SensorData(SENSOR_2, SENSOR_BLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_UNKNOWN,
                      sm->getCurrentCapture()->getCaptureState());
    sm->add(SensorData(SENSOR_3, SENSOR_BLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_READY,
                      sm->getCurrentCapture()->getCaptureState());

    // No records at this point
    TEST_ASSERT_EQUAL(0, sm->getCurrentCapture()->getData().size());

    // Add a deblock on sensor 3: We should go back to unknown
    sm->add(SensorData(SENSOR_3, SENSOR_UNBLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_UNKNOWN,
                      sm->getCurrentCapture()->getCaptureState());

    // Back to ready
    sm->add(SensorData(SENSOR_3, SENSOR_BLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_READY,
                      sm->getCurrentCapture()->getCaptureState());
    TEST_ASSERT_EQUAL(0, sm->getCurrentCapture()->getData().size());

    // Start capture - 1 elem
    sm->add(SensorData(SENSOR_1, SENSOR_UNBLOCKED, ts++));
    TEST_ASSERT_EQUAL(CAPTURE_STARTED,
                      sm->getCurrentCapture()->getCaptureState());
    TEST_ASSERT_EQUAL(1, sm->getCurrentCapture()->getData().size());

    // open all
    sm->add(SensorData(SENSOR_2, SENSOR_UNBLOCKED, ts++));
    sm->add(SensorData(SENSOR_3, SENSOR_UNBLOCKED, ts++));
    TEST_ASSERT_EQUAL(3, sm->getCurrentCapture()->getData().size());
    TEST_ASSERT_EQUAL(CAPTURE_STARTED,
                      sm->getCurrentCapture()->getCaptureState());
    // We were full open
    TEST_ASSERT_EQUAL(CAPTURE_FLAG_FULLOPEN,
                      sm->getCurrentCapture()->getCaptureFlags());

    sm->add(SensorData(SENSOR_1, SENSOR_BLOCKED, ts++));
    sm->add(SensorData(SENSOR_2, SENSOR_BLOCKED, ts++));
    sm->add(SensorData(SENSOR_3, SENSOR_BLOCKED, ts++));
    TEST_ASSERT_EQUAL(6, sm->getCurrentCapture()->getData().size());
    TEST_ASSERT_EQUAL(CAPTURE_DONE, sm->getCurrentCapture()->getCaptureState());
    // no bounce
    TEST_ASSERT_EQUAL(CAPTURE_FLAG_FULLOPEN,
                      sm->getCurrentCapture()->getCaptureFlags());

    sm->add(SensorData(SENSOR_3, SENSOR_UNBLOCKED, ts++));
    // we bounced
    TEST_ASSERT_NOT_EQUAL(CAPTURE_FLAG_FULLOPEN,
                          sm->getCurrentCapture()->getCaptureFlags());
    // Still open
    TEST_ASSERT_TRUE(sm->getCurrentCapture()->getCaptureFlags() &&
                     CAPTURE_FLAG_FULLOPEN);
    TEST_ASSERT_TRUE(sm->getCurrentCapture()->getCaptureFlags() &&
                     CAPTURE_FLAG_BOUNCE);
    TEST_ASSERT_TRUE(sm->getCurrentCapture()->getCaptureFlags() &&
                     CAPTURE_FLAG_EBOUNCE);
    sm->add(SensorData(SENSOR_3, SENSOR_BLOCKED, ts++));

    // We didn't add any events for the bounce
    TEST_ASSERT_EQUAL(6, sm->getCurrentCapture()->getData().size());

    TEST_ASSERT_EQUAL(SC_READY, sm->getCurrentCaptureStatus());
    TEST_ASSERT_EQUAL(1, sm->getCaptures().size());
    // close pending capture
    sm->yield(ts - 1);
    // We switched current capture
    TEST_ASSERT_EQUAL(2, sm->getCaptures().size());
    TEST_ASSERT_EQUAL(SC_DONE, sm->getCapture(1)->getStatus());
    TEST_ASSERT_EQUAL(SC_READY, sm->getCurrentCaptureStatus());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_singleton);
    RUN_TEST(test_sensorManagerState);
    RUN_TEST(test_addSensorDataAndStateChange);

    UNITY_END();
}
