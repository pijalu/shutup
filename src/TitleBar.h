#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <TFT_eSPI.h>

#include "button.h"
#include "gui_manager.h"
#include "utils.h"

class TitleBar : public Component {
private:
    const char* title;  // Title of the page
    int backgroundColor, borderColor, textColor;

    Button btn1 = Button("<", 0, 0, 18, getHeight());
    Button btn2 = Button(">", getWidth() - 18, 0, 18, getHeight());

    bool btn1Clicked(int x, int y) {
        log_i("Btn1 clicked");
        return true;
    }
    bool btn2Clicked(int x, int y) {
        log_i("Btn2 clicked");
        return true;
    }

public:
    TitleBar(const char* title, int x, int y, int width, int height,
             int backgroundColor = TFT_DARKGREY,
             int borderColor = TFT_LIGHTGREY, int textColor = TFT_WHITE)
        : Component(x, y, width, height),
          title(title),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
    }

    bool onClick(int x, int y) override {
        if (btn1.click(x, y))
            return btn1Clicked(x, y);
        if (btn2.click(x, y))
            return btn2Clicked(x, y);
        return false;
    }

    void onRender() {
        TFT_eSprite spr(getTft());
        spr.setColorDepth(8);  // 4 colors
        spr.createSprite(getWidth(), getHeight());
        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);

        spr.fillSprite(backgroundColor);
        spr.drawRect(0, 0, getWidth(), getHeight(), borderColor);

        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);
        spr.setTextColor(textColor);
        spr.setTextDatum(MC_DATUM);
        spr.setTextColor(textColor);
        spr.setTextSize(1);
        spr.drawString(title, getWidth() / 2, getHeight() / 2);

        btn1.onRender(spr);
        btn2.onRender(spr);

        spr.pushSprite(getX(), getY());
        spr.deleteSprite();
    }
};
#endif  // TITLEBAR_H