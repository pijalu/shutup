#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

void setTft(TFT_eSPI* tft);
TFT_eSPI* getTft();

void halt(String error);
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

#endif  // UTILS_H