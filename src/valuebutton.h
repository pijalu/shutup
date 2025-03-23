#ifndef VALUEBUTTON_H
#define VALUEBUTTON_H

#include <TFT_eSPI.h>

#include "gui_manager.h"
#include "utils.h"

#define VB_VALUECHANGE 100

class ValueButton : public Component {
public:
    ValueButton(const char* label, int value, const char* fmt, int max_value,
                int min_value, int step, int x, int y, int width, int height,
                int backgroundColor = TFT_DARKGREY,
                int borderColor = TFT_DARKGREY, int textColor = TFT_WHITE)
        : Component(x, y, width, height),
          label(label),
          value(value),
          fmt(fmt),
          max_value(max_value),
          min_value(min_value),
          step(step),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
        log_d("ValueButton created with label: %s - value: %d ; minvalue=%d ; "
              "maxvalue=%d, step=%d",
              label, value, min_value, max_value, step);

        plusBtn = new Rect(getX() + getWidth() - 20, getY(), 15, getHeight());
        minusBtn = new Rect(getX() + getWidth() - 55, getY(), 15, getHeight());
    }

    bool onClick(int x, int y) override {
        if (plusBtn->contains(x, y)) {
            value += step;
            if (value > max_value)
                value = max_value;
            redraw = true;
            notifyHandlers(EVT_NOTIFY, VB_VALUECHANGE, value);
        } else if (minusBtn->contains(x, y)) {
            value -= step;
            if (value < min_value)
                value = min_value;
            redraw = true;
            notifyHandlers(EVT_NOTIFY, VB_VALUECHANGE, value);
        }
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

        spr.drawString(utl_sprintf(fmt, value), getWidth() - 35,
                       getHeight() / 2);

        if (value > min_value) {
            spr.fillRect(minusBtn->x() - getX(), minusBtn->y() - getY(),
                         minusBtn->width(), minusBtn->height(), TFT_LIGHTGREY);
            spr.drawRect(getWidth() - 55, 0, 15, getHeight(), TFT_WHITE);
            spr.drawString("<", getWidth() - 50, getHeight() / 2);
        }

        if (value < max_value) {
            spr.fillRect(plusBtn->x() - getX(), plusBtn->y() - getY(),
                         plusBtn->width(), plusBtn->height(), TFT_LIGHTGREY);
            spr.drawRect(getWidth() - 20, 0, 15, getHeight(), TFT_WHITE);
            spr.drawString(">", getWidth() - 15, getHeight() / 2);
        }

        spr.pushSprite(getX(), getY());
        spr.deleteSprite();
    }

private:
    const char* label;  // btn label
    const char* fmt;    // formatting
    int value = 0;      // toggle state
    int max_value = 0;
    int min_value = 0;
    int step = 1;

    Rect *plusBtn, *minusBtn;

    bool redraw = true;
    int backgroundColor, borderColor, textColor;
};

#endif