#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 128, 160);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // pnlDateTime
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_date_time = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 128, 30);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlStatus
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_status = obj;
            lv_obj_set_pos(obj, 0, 30);
            lv_obj_set_size(obj, 128, 12);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlLightCtrl
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_light_ctrl = obj;
            lv_obj_set_pos(obj, 0, 42);
            lv_obj_set_size(obj, 128, 63);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlTempHumid
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_temp_humid = obj;
            lv_obj_set_pos(obj, 0, 105);
            lv_obj_set_size(obj, 128, 55);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_time = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 80, 30);
            lv_label_set_text(obj, "23:59");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_g7_segment7_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffb4ffb4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblMonth
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_month = obj;
            lv_obj_set_pos(obj, 80, 15);
            lv_obj_set_size(obj, 24, 15);
            lv_label_set_text(obj, "DEC");
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff0f), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblWeekDay
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_week_day = obj;
            lv_obj_set_pos(obj, 80, 0);
            lv_obj_set_size(obj, 24, 15);
            lv_label_set_text(obj, "DUM");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff0f), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTempTxt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_temp_txt = obj;
            lv_obj_set_pos(obj, 0, 105);
            lv_obj_set_size(obj, 69, 15);
            lv_label_set_text(obj, "Temperature");
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblHumiTxt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_humi_txt = obj;
            lv_obj_set_pos(obj, 69, 105);
            lv_obj_set_size(obj, 59, 15);
            lv_label_set_text(obj, "Humidity");
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTempInt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_temp_int = obj;
            lv_obj_set_pos(obj, 10, 120);
            lv_obj_set_size(obj, 44, 40);
            lv_label_set_text(obj, "23");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_open_sans_bold_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblHumiVal
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_humi_val = obj;
            lv_obj_set_pos(obj, 69, 120);
            lv_obj_set_size(obj, 44, 40);
            lv_label_set_text(obj, "69");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_open_sans_bold_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTempSign
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_temp_sign = obj;
            lv_obj_set_pos(obj, 2, 120);
            lv_obj_set_size(obj, 10, 40);
            lv_label_set_text(obj, "+");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTempUnit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_temp_unit = obj;
            lv_obj_set_pos(obj, 54, 120);
            lv_obj_set_size(obj, 15, 20);
            lv_label_set_text(obj, "°C");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTempDec
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_temp_dec = obj;
            lv_obj_set_pos(obj, 54, 140);
            lv_obj_set_size(obj, 15, 20);
            lv_label_set_text(obj, "8");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_open_sans_bold_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblHumiUnit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_humi_unit = obj;
            lv_obj_set_pos(obj, 113, 120);
            lv_obj_set_size(obj, 15, 40);
            lv_label_set_text(obj, "%");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblWifiStatus
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_wifi_status = obj;
            lv_obj_set_pos(obj, 0, 30);
            lv_obj_set_size(obj, 50, 12);
            lv_label_set_text(obj, "WiFi:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblStatus
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_status = obj;
            lv_obj_set_pos(obj, 64, 30);
            lv_obj_set_size(obj, 52, 12);
            lv_label_set_text(obj, "Status:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ledStatus
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_status = obj;
            lv_obj_set_pos(obj, 116, 34);
            lv_obj_set_size(obj, 6, 6);
            lv_led_set_color(obj, lv_color_hex(4278255487));
            lv_led_set_brightness(obj, 255);
        }
        {
            // lblDay
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_day = obj;
            lv_obj_set_pos(obj, 104, 0);
            lv_obj_set_size(obj, 24, 15);
            lv_label_set_text(obj, "99");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff0f), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblYear
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_year = obj;
            lv_obj_set_pos(obj, 104, 15);
            lv_obj_set_size(obj, 24, 15);
            lv_label_set_text(obj, "9999");
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff0f), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblBrightValue
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_bright_value = obj;
            lv_obj_set_pos(obj, 64, 57);
            lv_obj_set_size(obj, 48, 48);
            lv_label_set_text(obj, "0");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_open_sans_bold_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblSwitchText
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_switch_text = obj;
            lv_obj_set_pos(obj, 0, 42);
            lv_obj_set_size(obj, 96, 15);
            lv_label_set_text(obj, "Light switch:");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff7fff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblBrightUnit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_bright_unit = obj;
            lv_obj_set_pos(obj, 113, 58);
            lv_obj_set_size(obj, 15, 47);
            lv_label_set_text(obj, "%");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // imgLightState
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.img_light_state = obj;
            lv_obj_set_pos(obj, 8, 57);
            lv_obj_set_size(obj, 48, 48);
            lv_img_set_src(obj, &img_turned_off_48);
        }
        {
            // ledSwitchState
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_switch_state = obj;
            lv_obj_set_pos(obj, 116, 47);
            lv_obj_set_size(obj, 6, 6);
            lv_led_set_color(obj, lv_color_hex(4294902015));
            lv_led_set_brightness(obj, 255);
        }
        {
            // imgWiFiStat
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.img_wi_fi_stat = obj;
            lv_obj_set_pos(obj, 52, 30);
            lv_obj_set_size(obj, 12, 12);
            lv_img_set_src(obj, &img_wifi_off_12);
        }
    }
}

void tick_screen_main_screen() {
}

void create_screen_init_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.init_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 128, 160);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lblTitle
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_title = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 128, 16);
            lv_label_set_text(obj, "Light control");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblInitStatus
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_init_status = obj;
            lv_obj_set_pos(obj, 0, 128);
            lv_obj_set_size(obj, 128, 32);
            lv_label_set_text(obj, "Initializing");
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbeffa0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff00ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // imgLightBulb
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.img_light_bulb = obj;
            lv_obj_set_pos(obj, 16, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_lightbulb_96);
        }
    }
}

void tick_screen_init_screen() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main_screen();
    create_screen_init_screen();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main_screen,
    tick_screen_init_screen,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
