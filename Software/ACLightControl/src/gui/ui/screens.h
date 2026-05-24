#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main_screen;
    lv_obj_t *init_screen;
    lv_obj_t *img_light_bulb;
    lv_obj_t *img_light_state;
    lv_obj_t *img_wi_fi_stat;
    lv_obj_t *lbl_bright_unit;
    lv_obj_t *lbl_bright_value;
    lv_obj_t *lbl_day;
    lv_obj_t *lbl_humi_txt;
    lv_obj_t *lbl_humi_unit;
    lv_obj_t *lbl_humi_val;
    lv_obj_t *lbl_init_status;
    lv_obj_t *lbl_month;
    lv_obj_t *lbl_status;
    lv_obj_t *lbl_switch_text;
    lv_obj_t *lbl_temp_dec;
    lv_obj_t *lbl_temp_int;
    lv_obj_t *lbl_temp_sign;
    lv_obj_t *lbl_temp_txt;
    lv_obj_t *lbl_temp_unit;
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_title;
    lv_obj_t *lbl_week_day;
    lv_obj_t *lbl_wifi_status;
    lv_obj_t *lbl_year;
    lv_obj_t *led_status;
    lv_obj_t *led_switch_state;
    lv_obj_t *pnl_date_time;
    lv_obj_t *pnl_light_ctrl;
    lv_obj_t *pnl_status;
    lv_obj_t *pnl_temp_humid;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_SCREEN = 1,
    SCREEN_ID_INIT_SCREEN = 2,
};

void create_screen_main_screen();
void tick_screen_main_screen();

void create_screen_init_screen();
void tick_screen_init_screen();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/