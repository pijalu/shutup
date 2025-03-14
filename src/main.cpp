#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h> 


#include "defs.h"
#include "touch.h"
#include "utils.h"
#include "ui.h"

#include "NotoSansBold15.h"
#include "NotoSansBold36.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
#define SENSOR_1_PIN 34
#define SENSOR_2_PIN 35
#define SENSOR_3_PIN 32
#define LASERS_PIN 12

static int sensor_setup_error = 0;

static int sensor_1 = 0;
void IRAM_ATTR isr_sensor_1() {
	sensor_1 = !sensor_1;
  Serial.printf("Sensor 1: %d\r\n", sensor_1);
}

static int sensor_2 = 0;
void IRAM_ATTR isr_sensor_2() {
	sensor_2 = !sensor_2;
  Serial.printf("Sensor 2: %d\r\n", sensor_2);
}

static int sensor_3 = 0;
void IRAM_ATTR isr_sensor_3() {
	sensor_3 = !sensor_3;
  Serial.printf("Sensor 3: %d\r\n", sensor_3);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");

  Serial.println("Starting tft system");
  tft.init();
  tft.setRotation(TFT_ROTATION);
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
  Serial.printf("Sensor initial status: s1: %d - s2: %d - s3: %d\r\n", sensor_1, sensor_2, sensor_3);

  if (sensor_1 || sensor_2 || sensor_3) {
    Serial.println("Warning: Some sensors are providing UNBLOCKED status without any active lasers !");
    sensor_setup_error++;
  }

  Serial.println("Set lasers on");
  digitalWrite(12, HIGH);

  sensor_1 = !digitalRead(SENSOR_1_PIN);
  sensor_2 = !digitalRead(SENSOR_2_PIN);
  sensor_3 = !digitalRead(SENSOR_3_PIN);

  Serial.printf("Sensor post lasers status: s1: %d - s2: %d - s3: %d\r\n", sensor_1, sensor_2, sensor_3);
  if (!sensor_1 || !sensor_2 || !sensor_3) {
    Serial.println("Warning: Some sensors are providing BLOCKED status with active lasers !");
    sensor_setup_error++;
  }

  attachInterrupt(digitalPinToInterrupt(SENSOR_1_PIN), isr_sensor_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_2_PIN), isr_sensor_2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_3_PIN), isr_sensor_3, CHANGE);

  Serial.println("Start up finished");
}

void loop() {
#if 0
  ui_loop();
#endif

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.setCursor(0, 0); // Set cursor at top left of screen
  //tft.loadFont(AA_FONT_SMALL);    // Must load the font first

  if (sensor_setup_error) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.printf("Warning: %d errors on sensor init\n", sensor_setup_error);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("Probably ready to rumble...\n");
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Ready to Rumble"); // println moves cursor down for a new line
  }
  //tft.unloadFont(); // Remove the font to recover memory used
  delay(8000);
}
