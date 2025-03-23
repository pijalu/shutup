#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <TFT_eSPI.h>

#include "gui_manager.h"
#include "utils.h"

class ToggleButton : public Component {
public:
    ToggleButton(const char* label, bool isOn, int x, int y, int width,
                 int height, int backgroundColor = TFT_DARKGREY,
                 int borderColor = TFT_DARKGREY, int textColor = TFT_WHITE)
        : Component(x, y, width, height),
          label(label),
          isOn(isOn),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
    }

    bool onClick(int x, int y) override {
        isOn = !isOn;
        redraw = true;

        // handled
        return true;
    }

    void onHide() override {
        // force redraw
        redraw = true;
    }

    // bypass subrender
    void onRender() override {
        if (!redraw)
            return;
        redraw = false;

        TFT_eSprite spr(getTft());
        // spr.setColorDepth(8);  // 4 colors
        spr.createSprite(getWidth(), getHeight());
        spr.fillSprite(backgroundColor);
        if (backgroundColor != borderColor)
            spr.drawRect(0, 0, getWidth(), getHeight(), borderColor);

        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);

        spr.setTextDatum(ML_DATUM);
        spr.setTextColor(textColor);
        spr.drawString(label, 5, getHeight() / 2);

        // spr.fillRoundRect(getWidth() - 25, 5, 20, 5, 5, TFT_BLUE);
        // spr.fillRect(getWidth() - 25, getHeight() / 2 - 5, 21, 11, TFT_BLUE);
        spr.fillRoundRect(getWidth() - 25, (getHeight() / 2) - 5, 21, 11, 5,
                          TFT_LIGHTGREY);

        if (isOn)
            spr.fillCircle(getWidth() - 10, getHeight() / 2, 5, TFT_DARKGREEN);
        else
            spr.fillCircle(getWidth() - 20, getHeight() / 2, 5, TFT_RED);

        spr.pushSprite(getX(), getY());
        spr.deleteSprite();
    }

private:
    const char* label;  // btn label
    bool isOn = false;  // toggle state
    bool redraw = true;
    int backgroundColor, borderColor, textColor;
};

#endif