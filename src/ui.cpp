#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

#include "defs.h"
#include "touch.h"
#include "utils.h"

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    lv_display_flush_ready(disp);
}

// Get the Touchscreen data
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    u_int16_t x, y, z;
    if (getTouch(&x, &y, &z))
    {
        data->state = LV_INDEV_STATE_PRESSED;
        // Set the coordinates
        data->point.x = x;
        data->point.y = y;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_2(void)
{
    lv_obj_t *btn = lv_button_create(lv_screen_active());       /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                                /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                              /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

    lv_obj_t *label = lv_label_create(btn); /*Add a label to the button*/
    lv_label_set_text(label, "Button");     /*Set the labels text*/
    lv_obj_center(label);
}

static void draw_event_cb(lv_event_t *e)
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    /*If the cells are drawn...*/
    if (base_dsc->part == LV_PART_ITEMS)
    {
        uint32_t row = base_dsc->id1;
        uint32_t col = base_dsc->id2;

        /*Make the texts in the first cell center aligned*/
        if (row == 0)
        {
            lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if (label_draw_dsc)
            {
                label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
            }
            lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if (fill_draw_dsc)
            {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
                fill_draw_dsc->opa = LV_OPA_COVER;
            }
        }
        /*In the first column align the texts to the right*/
        else if (col == 0)
        {
            lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if (label_draw_dsc)
            {
                label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
            }
        }

        /*Make every 2nd row grayish*/
        if ((row != 0 && row % 2) == 0)
        {
            lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if (fill_draw_dsc)
            {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), fill_draw_dsc->color, LV_OPA_10);
                fill_draw_dsc->opa = LV_OPA_COVER;
            }
        }
    }
}

#define PC2PX_W(X) ((TFT_WIDTH / 100.0) * (X))

void lv_example_table_1(void)
{
    lv_obj_t *table = lv_table_create(lv_screen_active());

    static lv_style_t yourName;
    lv_style_init(&yourName);
    lv_style_set_text_font(&yourName, &lv_font_unscii_8);
    lv_style_set_pad_all(&yourName, 1);
    lv_obj_add_style(table, &yourName, 0);

    /*Fill the first column*/
    lv_table_set_cell_value(table, 0, 0, "Fn");
    lv_table_set_cell_value(table, 1, 0, "A");
    lv_table_set_cell_value(table, 2, 0, "B");
    lv_table_set_cell_value(table, 3, 0, "C");

    /*Fill the second column*/
    lv_table_set_cell_value(table, 0, 1, "S1");
    lv_table_set_cell_value(table, 1, 1, "1 / 1500");
    lv_table_set_cell_value(table, 2, 1, "1 / 1500");
    lv_table_set_cell_value(table, 3, 1, "1 / 1500");

    lv_table_set_cell_value(table, 0, 2, "M");
    lv_table_set_cell_value(table, 1, 2, "1 / 1500");
    lv_table_set_cell_value(table, 2, 2, "1 / 1500");
    lv_table_set_cell_value(table, 3, 2, "1 / 1500");

    lv_table_set_cell_value(table, 0, 3, "S3");
    lv_table_set_cell_value(table, 1, 3, "1 / 1500");
    lv_table_set_cell_value(table, 2, 3, "1 / 1500");
    lv_table_set_cell_value(table, 3, 3, "1 / 1500");

    lv_table_set_col_width(table, 0, PC2PX_W(15));
    lv_table_set_col_width(table, 1, PC2PX_W(28));
    lv_table_set_col_width(table, 2, PC2PX_W(28));
    lv_table_set_col_width(table, 3, PC2PX_W(28));

    lv_obj_center(table);
    // lv_obj_set_size(table, lv_pct(100), lv_pct(100));

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
}

void ui_setup()
{
    String LVGL_startup = "Setting up LVGL ";
    LVGL_startup += String("V") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_startup);
    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    /* register print function for debugging */
#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
#endif

    lv_display_t *disp;
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    /*Initialize the (dummy) input device driver*/
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, touchscreen_read);

    lv_example_table_1();
}

void ui_loop()
{
    lv_timer_handler(); /* let the GUI do its work */
}