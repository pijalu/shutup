#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

#include <list>

#include "GraphDisplay.h"
#include "SensorManager.h"
#include "defs.h"
#include "touch.h"
#include "ui.h"
#include "utils.h"

TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
#define SENSOR_1_PIN 34
#define SENSOR_2_PIN 35
#define SENSOR_3_PIN 32
#define LASERS_PIN 12

GraphDisplay* graph_display;
static int sensor_setup_error = 0;
static int sensor_1 = 0;
static int sensor_2 = 0;
static int sensor_3 = 0;

void IRAM_ATTR isr_sensor_1() {
    char buffer[50];
    sensor_1 = !sensor_1;
    auto data = SensorData(sensor_1, millis());
    SensorManager::getInstance()->addToSensorSeries(0, data);
    Serial.printf("Sensor 1: %s\r\n", data.toString(buffer, sizeof(buffer)));
}

void IRAM_ATTR isr_sensor_2() {
    char buffer[50];
    sensor_2 = !sensor_2;
    auto data = SensorData(sensor_2, millis());
    SensorManager::getInstance()->addToSensorSeries(1, data);
    Serial.printf("Sensor 1: %s\r\n", data.toString(buffer, sizeof(buffer)));
}

void IRAM_ATTR isr_sensor_3() {
    char buffer[50];
    sensor_3 = !sensor_3;
    auto data = SensorData(sensor_3, millis());
    SensorManager::getInstance()->addToSensorSeries(2, data);
    Serial.printf("Sensor 1: %s\r\n", data.toString(buffer, sizeof(buffer)));
}

void setup() {
    std::list<int> shut1;
    shut1.push_front(1);

    Serial.begin(115200);
    Serial.println("Starting up");

    Serial.println("Starting tft system");
    tft.init();
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(TFT_BLACK);

    // For possible errors
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_RED, TFT_BLACK);
#if 0
  Serial.println("Setting up touch subsystem");
  touchSetup();

  ui_setup();
#endif

    pinMode(LASERS_PIN, OUTPUT);
    pinMode(SENSOR_1_PIN, INPUT);
    pinMode(SENSOR_2_PIN, INPUT);
    pinMode(SENSOR_3_PIN, INPUT);

    sensor_1 = !digitalRead(SENSOR_1_PIN);
    sensor_2 = !digitalRead(SENSOR_2_PIN);
    sensor_3 = !digitalRead(SENSOR_3_PIN);
    Serial.printf("Sensor initial status: s1: %d - s2: %d - s3: %d\r\n",
                  sensor_1, sensor_2, sensor_3);

    if (sensor_1 || sensor_2 || sensor_3) {
        tft.printf("Warning: Some sensors are providing UNBLOCKED status "
                   "without any active lasers !\n");
        tft.printf("s1:%d/s2:%d/s3:%d\n", sensor_1, sensor_2, sensor_3);

        Serial.println("Warning: Some sensors are providing UNBLOCKED status "
                       "without any active lasers !");
        sensor_setup_error++;
    }

    Serial.println("Set lasers on");
    digitalWrite(12, HIGH);
    delay(1);

    sensor_1 = !digitalRead(SENSOR_1_PIN);
    sensor_2 = !digitalRead(SENSOR_2_PIN);
    sensor_3 = !digitalRead(SENSOR_3_PIN);

    Serial.printf("Sensor post lasers status: s1: %d - s2: %d - s3: %d\r\n",
                  sensor_1, sensor_2, sensor_3);
    if (!sensor_1 || !sensor_2 || !sensor_3) {
        tft.printf("Warning: Some sensors are providing BLOCKED status with "
                   "active lasers !\n");
        tft.printf("s1:%d/s2:%d/s3:%d\n", sensor_1, sensor_2, sensor_3);

        Serial.println("Warning: Some sensors are providing BLOCKED status "
                       "with active lasers !");
        sensor_setup_error++;
    }
    if (sensor_setup_error) {
        tft.printf("Warning: %d errors on sensor init\n", sensor_setup_error);

#ifndef SIM
        delay(5000);
#endif
        // Clear screen
        tft.fillScreen(TFT_BLACK);
    }

    attachInterrupt(digitalPinToInterrupt(SENSOR_1_PIN), isr_sensor_1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSOR_2_PIN), isr_sensor_2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSOR_3_PIN), isr_sensor_3, CHANGE);

    graph_display =
        new GraphDisplay(tft, 10, 50, tft.width() - 20, 100, 10 * 1000);

    Serial.println("Start up finished");
}

void loop() {
#if 0
  ui_loop();
#endif

    // tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, 0);             // Set cursor at top left of screen
    tft.println("Ready to Rumble");  // println moves cursor down for a new line

    graph_display->update();
    yield();
    delay(500);  // Delay between updates
}
