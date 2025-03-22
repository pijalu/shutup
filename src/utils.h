#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

void setTft(TFT_eSPI* tft);
TFT_eSPI* getTft();

void init_fs();
void init_config();

void halt(String error);
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

char* utl_sprintf(const char* format, ...);

int read_sensor(int pin);

#endif  // UTILS_H