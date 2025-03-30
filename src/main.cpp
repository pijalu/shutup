#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <list>

#include "SensorManager.h"
#include "config.h"
#include "config_manager.h"
#include "defs.h"
#include "touch.h"
#include "ui.h"
#include "utils.h"

#define SENSOR_1_PIN 34
#define SENSOR_2_PIN 35
#define SENSOR_3_PIN 32
#define LASERS_PIN 12

static int sensor_setup_error = 0;
static int sensor_1 = 0;
static int sensor_2 = 0;
static int sensor_3 = 0;

void IRAM_ATTR isr_sensor_1() {
    char buffer[50];
    sensor_1 = !sensor_1;
    SensorManager::getInstance()->add(SensorData(SENSOR_1, sensor_1, millis()));
}

void IRAM_ATTR isr_sensor_2() {
    char buffer[50];
    sensor_2 = !sensor_2;
    SensorManager::getInstance()->add(SensorData(SENSOR_2, sensor_2, millis()));
}

void IRAM_ATTR isr_sensor_3() {
    char buffer[50];
    sensor_3 = !sensor_3;
    SensorManager::getInstance()->add(SensorData(SENSOR_3, sensor_3, millis()));
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting up");

    init_fs();

    init_config();  // set default config
    ui_setup();

    pinMode(LASERS_PIN, OUTPUT);
    pinMode(SENSOR_1_PIN, INPUT);
    pinMode(SENSOR_2_PIN, INPUT);
    pinMode(SENSOR_3_PIN, INPUT);

    sensor_1 = read_sensor(SENSOR_1_PIN);
    sensor_2 = read_sensor(SENSOR_2_PIN);
    sensor_3 = read_sensor(SENSOR_3_PIN);
    log_d("Sensor initial status: s1: %d - s2: %d - s3: %d\r\n", sensor_1,
          sensor_2, sensor_3);

    if (sensor_1 || sensor_2 || sensor_3) {
        log_w("Warning: Some sensors are providing UNBLOCKED status "
              "without any active lasers !");
        sensor_setup_error++;
    }

    log_d("Set lasers on");
    digitalWrite(12, HIGH);
    delay(1);

    sensor_1 = read_sensor(SENSOR_1_PIN);
    sensor_2 = read_sensor(SENSOR_2_PIN);
    sensor_3 = read_sensor(SENSOR_3_PIN);

    log_d("Sensor post lasers status: s1: %d - s2: %d - s3: %d\r\n", sensor_1,
          sensor_2, sensor_3);
    if (!sensor_1 || !sensor_2 || !sensor_3) {
        log_w("Warning: Some sensors are providing BLOCKED status "
              "with active lasers !");
        sensor_setup_error++;
    }

    // Set current status
    auto now = millis();
    SensorManager::getInstance()->add(SensorData(SENSOR_1, sensor_1, now));
    SensorManager::getInstance()->add(SensorData(SENSOR_2, sensor_2, now));
    SensorManager::getInstance()->add(SensorData(SENSOR_3, sensor_3, now));

    // Attach to ISR
    attachInterrupt(digitalPinToInterrupt(SENSOR_1_PIN), isr_sensor_1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSOR_2_PIN), isr_sensor_2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSOR_3_PIN), isr_sensor_3, CHANGE);

    log_i("Start up finished");

    log_d("PSRAM      : %05d", ESP.getFreePsram());
    log_d("HEAP       : %05d", ESP.getFreeHeap());
    log_d("Min. PSRAM : %05d", ESP.getMinFreePsram());
    log_d("Min. HEAP  : %05d", ESP.getMinFreeHeap());
}

void loop() {
    ui_loop();
    int max_measure_duration =
        ConfigManager::getInstance().getInt(MAX_MEASURE_DURATION);
    int max_measure_bounce =
        ConfigManager::getInstance().getInt(MAX_MEASURE_BOUNCE);

    SensorManager::getInstance()->yield(millis(), max_measure_duration,
                                        max_measure_bounce);
#ifdef SIM
    {
        // return;
        static int lastUpdate = 0;
        static int sensor = 0;
        if (millis() - lastUpdate > 500) {  // sim change of state every 500ms
            lastUpdate = millis();
            switch (sensor) {
                case 0:
                    isr_sensor_1();
                    break;
                case 1:
                    isr_sensor_2();
                    break;
                case 2:
                    isr_sensor_3();
                    break;
            }
            sensor = (sensor + 1) % MAX_SENSOR;
        }
    }
#endif
}
