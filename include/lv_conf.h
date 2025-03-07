#if defined(ILI9488_DRIVER)
#include "lv_conf_ili9488.h"
#elif defined(ILI9341_DRIVER)
#include "lv_conf_ili9341.h"
#else
#pragma error "No screen config..."
#endif
