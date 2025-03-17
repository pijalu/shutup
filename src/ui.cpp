#include "ui.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "GraphDisplay.h"
#include "StatusBar.h"
#include "TitleBar.h"
#include "gui_manager.h"
#include "touch.h"
#include "ui.h"
#include "utils.h"

#define TARGET_FPS 10
#define MIN_FPS 5

class GUI : public UI {
public:
    GUI() : UI() {
    }
};

GraphDisplay* graph_display;
GUI* gui = nullptr;

class UIPage : public Page {
public:
    UIPage(const char* title)
        : Page(title, 0, 0, getTft()->width(), getTft()->height()) {
        add(new StatusBar(0, getTft()->height() - 20, getTft()->width(), 20));
        add(new TitleBar(title, 0, 0, getTft()->width(), 20));
    }
};

void ui_setup() {
    log_i("Starting tft system");
    TFT_eSPI* tft = new TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
    setTft(tft);

    tft->init();
    tft->setRotation(0);
    tft->fillScreen(TFT_DARKGREY);

    // Initialize touch screen
    log_i("Starting touch system");
    touchSetup();

    log_i("Starting UI management");
    gui = new GUI();

    Page* sensors_page = new UIPage("Sensors");
    sensors_page->add(new GraphDisplay(10, getTft()->height() - 75,
                                       getTft()->width() - 20, 50, 10000));

    gui->addPage(sensors_page);

    gui->addPage(new UIPage("Settings"));
    gui->addPage(new UIPage("About"));

    gui->setActivePage(0);  // Sensors page

    // done
    log_i("ui setup complete");
}

void ui_loop() {
    static int fps = TARGET_FPS;
#define FRAME_MS_DURATION (1000 / fps)

    int start = millis();

    gui->render();
    u_int16_t x, y, z;
    if (getTouch(&x, &y, &z)) {
        log_i("touch at (%d, %d @ %d)", x, y, z);
        gui->click(x, y);
    }
    yield();

    int duration = millis() - start;
    if (duration < FRAME_MS_DURATION) {
        delay(FRAME_MS_DURATION - duration);
    } else {
        // adapt FPS
        fps = MAX((1000 / duration) - 1, 10);
        log_i("FPS set to: %d", fps);
    }
#undef FRAME_MS_DURATION
}