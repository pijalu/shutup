#include <Arduino.h>

// Halt processing
void halt(String error) {
    Serial.printf("Halted: %s", error.c_str());
    while (true) {
        delay(1000);
    }
}

