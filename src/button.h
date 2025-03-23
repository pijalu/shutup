#ifndef BUTTON_H
#define BUTTON_H

#include <TFT_eSPI.h>

#include "gui_manager.h"
#include "utils.h"

class Button : public Component {
public:
    Button(const char* label, int x, int y, int width, int height,
           int backgroundColor = TFT_DARKGREY, int borderColor = TFT_LIGHTGREY,
           int textColor = TFT_WHITE)
        : Component(x, y, width, height),
          label(label),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
    }

    // pass click to parent component
    bool onClick(int x, int y) override {
        return false;
    }

    // bypass subrender
    void onRender() override {
        throw std::runtime_error("Button::onRender not callable");
    }

    // render on an existing sprite
    void onRender(TFT_eSprite& spr) {
                spr.fillRect(getX(), getY(), getWidth(), getHeight(), backgroundColor);
        spr.drawRect(getX(), getY(), getWidth(), getHeight(), borderColor);

        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);
        spr.setTextDatum(MC_DATUM);
        spr.setTextColor(textColor);
        spr.setTextSize(1);
        spr.drawString(label, getX() + getWidth() / 2,
                       getY() + getHeight() / 2);
    }

private:
    const char* label;  // btn label
    int backgroundColor, borderColor, textColor;
};

#endif