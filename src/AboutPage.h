#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <TFT_eSPI.h>

#include "UIPage.h"
#include "defs.h"
#include "gui_manager.h"

class AboutPage : public UIPage {
public:
    AboutPage() : UIPage("About") {
    }

    void onRender() override {
        if (!getHiddenFlag()) {
            return;
        }
        setHiddenFlag(false);
        int y = 0;

        TFT_eSPI* tft = getTft();
        Rect r = getDrawRect();
        tft->fillRect(r.x(), r.y(), r.width(), r.height(), TFT_DARKGREY);

        tft->setTextColor(TFT_WHITE);
        tft->setTextFont(2);
        tft->setTextDatum(TC_DATUM);
        y += 25;
        tft->drawString(utl_sprintf("Version: %s", VERSION), tft->width() / 2,
                        y);
        y += 20;
        tft->drawString(
            utl_sprintf("Build Date: %s - %s", BUILD_DATE, BUILD_TIME),
            tft->width() / 2, y);

        y += 40;
        tft->setTextFont(1);
        tft->drawString(utl_sprintf("PSRAM      : %07d", ESP.getFreePsram()),
                        tft->width() / 2, y);
        y += 20;
        tft->drawString(utl_sprintf("HEAP       : %07d", ESP.getFreeHeap()),
                        tft->width() / 2, y);
        y += 20;
        tft->drawString(utl_sprintf("Min. PSRAM : %07d", ESP.getMinFreePsram()),
                        tft->width() / 2, y);
        y += 20;
        tft->drawString(utl_sprintf("Min. HEAP  : %07d", ESP.getMinFreeHeap()),
                        tft->width() / 2, y);
    }
};

#endif  // ABOUTPAGE_H