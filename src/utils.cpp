#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <map>

#include "config_manager.h"

void init_fs() {
#ifndef SIM
    if (!SPIFFS.begin()) {
        Serial.println("formatting file system");
        if (!SPIFFS.format()) {
            halt("Formatting failed !");
        }
        Serial.println("formatted done...");
        if (!SPIFFS.begin()) {
            halt("SPIFFS begin failed !");
        }
    }
#else
    log_d("Simulation - no SPIFFS");
#endif
}

void init_config() {
    // Set default configuration
    std::map<std::string, std::string> defaults;
    defaults[SENSOR_CLOSE_LEVEL] = "0";  // What the sensor return when closed

    // Load default
    ConfigManager::getInstance().setDefaultConfig(defaults);
    ConfigManager::getInstance().load();
}

int read_sensor(int pin) {
    static int sensor_config_close_level = -1;
    if (sensor_config_close_level == -1) {
        sensor_config_close_level =
            ConfigManager::getInstance().getInt(SENSOR_CLOSE_LEVEL, 0);
    }

    int sensor_value = digitalRead(pin);
    if (sensor_value == sensor_config_close_level) {
        return SENSOR_CLOSED;  // Sensor is closed
    } else {
        return SENSOR_OPEN;
    }
}

static TFT_eSPI* _tft = nullptr;
void setTft(TFT_eSPI* tft) {
    _tft = tft;
}
TFT_eSPI* getTft() {
    return _tft;
}

// Halt processing
void halt(String error) {
    Serial.printf("Halted: %s", error.c_str());
    while (true) {
        delay(1000);
    }
}

char* utl_sprintf(const char* format, ...) {
    static char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return buffer;
}
