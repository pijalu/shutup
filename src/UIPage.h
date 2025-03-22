#ifndef UI_PAGE_H
#define UI_PAGE_H

#include <TFT_eSPI.h>

#include "StatusBar.h"
#include "TitleBar.h"
#include "gui_manager.h"

class UIPage : public Page {
public:
    UIPage(const char *title)
        : Page(title, 0, 0, getTft()->width(), getTft()->height()) {
        add(new StatusBar(0, getTft()->height() - 20, getTft()->width(), 20));
        add(new TitleBar(title, 0, 0, getTft()->width(), 20));
    }
};

#endif  // UI_PAGE_H