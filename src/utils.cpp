#include <Arduino.h>
#include <TFT_eSPI.h>

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
