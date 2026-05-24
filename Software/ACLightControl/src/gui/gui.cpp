#include <Arduino.h>
#include "gui.h"
#include "assert.h"
#include "images.h"

// GUI object
GUI gui;

// Function to "push" the pixels on the physical display.
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
   gui.tft->startWrite();
   gui.tft->setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
   gui.tft->pushImage(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)px_map);
   gui.tft->endWrite();
   lv_display_flush_ready(disp); // Tell LVGL the flushing is done
}

#define LV_LED_BRIGHT_MIN      31
#define LV_LED_BRIGHT_MAX     255

GUI::GUI():
   init_color(LV_COLOR_BLACK),
   wifi_color(LV_COLOR_DARKBLUE),
   fail_color(LV_COLOR_RED),
   on_color(LV_COLOR_GREEN),
   off_color(MAKE_COLOR_HEX32(160,160,160)),
   light_icons{&img_turned_off_48, &img_turning_off_48, &img_turned_on_48, &img_turning_on_48},
   wifi_icons{&img_wifi_off_12, &img_wifi_on_12}
{
}

void GUI::showTemperature(double val)
{
   uint8_t integ = 0u;
   uint8_t digit = 0u;
   String int_str;
   String dig_str;
   String sign = "";
   
   // Set integral and decimal part of the temperature.
   if(val < 0.0)
   {
      // Get integer and digit part.
      integ = (uint8_t)(val * -1.0);
      digit = (uint8_t)(((val * -1.0) - integ) * 10u);
      // Set the sign.
      sign = "-";
   }
   else
   {
      // Get integer and digit part.
      integ = (uint8_t)val;
      digit = (uint8_t)((val - integ) * 10u);
      // Set the sign.
      sign = "+";
   }
   int_str = String(integ);
   dig_str = String(digit);

   // Show the sign.
   lv_label_set_text(objects.lbl_temp_sign, sign.c_str());
   // Show integer part.
   lv_label_set_text(objects.lbl_temp_int, int_str.c_str());
   // Show decimal part.
   lv_label_set_text(objects.lbl_temp_dec, dig_str.c_str());
}

void GUI::showHumidity(uint8_t val)
{
   // Show the value on label.
   lv_label_set_text(objects.lbl_humi_val, String(val).c_str());
}

void GUI::showLightBrightness(uint8_t val)
{
   String strVal = String(val);

   // Show the value on label.
   lv_label_set_text(objects.lbl_bright_value, strVal.c_str());
}

void GUI::showLightState(uint8_t state)
{
   // Make sure state is not 0.
   if (state == 0u)
   {
      state = 1u;
   }
   // Set the icon.
   lv_img_set_src(objects.img_light_state, light_icons[state-1u]);
}

void GUI::showSwitchState(bool state)
{
   uint8_t brigh = (state == true?LV_LED_BRIGHT_MAX:LV_LED_BRIGHT_MIN);

   // Set the brightness.
   lv_led_set_brightness(objects.led_switch_state, brigh);
}

void GUI::showWiFiState(bool state)
{
   uint8_t index = (state == true?1u:0u);

   // Set the icon.
   lv_img_set_src(objects.img_wi_fi_stat, wifi_icons[index]);
}

void GUI::showStatusLED(bool state)
{
   uint8_t brigh = (state == true?LV_LED_BRIGHT_MAX:LV_LED_BRIGHT_MIN);

   // Set the brightness.
   lv_led_set_brightness(objects.led_status, brigh);
}

void GUI::delay_lvgl_ui(uint32_t ms)
{
   uint32_t cnt = ms / 5;

   while (cnt > 0)
   {
      // Run LVGL task.
      lv_task_handler();  
      lv_tick_inc(5); 
      cnt--;
      delay(5);
   }
}

void GUI::begin(LGFX *tft_disp)
{
   // Assign the TFT object.
   tft = tft_disp;
   // Initialize LVGL system.
   lv_init();
   // Create a new display object with LVGL 9 API.
   lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
   // Set LVGL's draw buffers with single buffering.
   lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
   // Set the flush callback to handle drawing
   lv_display_set_flush_cb(disp, my_disp_flush);

   // Create LVGL widgets and show the main screen.
   ui_init();
}

void GUI::showInitScreen(const char* title)
{
   lv_label_set_text(objects.lbl_title, title);
   loadScreen(SCREEN_ID_INIT_SCREEN);
   delay_lvgl_ui(100);
}

void GUI::showMainWnd(void)
{
   loadScreen(SCREEN_ID_MAIN_SCREEN);
   delay_lvgl_ui(100);
}

void GUI::showTime(uint8_t hrs, uint8_t min)
{
   String time_str = "";
   if (hrs < 10)
   {
      time_str += "0";
   }
   time_str += String(hrs) + ":";
   if (min < 10)
   {
      time_str += "0";
   }
   time_str += String(min);
   // Show the time.
   lv_label_set_text(objects.lbl_time, time_str.c_str());
}

void GUI::showDate(uint8_t d, const char* m, uint16_t y, const char *wd)
{
   String day_str = "";
   if (d < 10)
   {
      day_str += "0";
   }
   day_str += String(d);

   // Show the day.
   lv_label_set_text(objects.lbl_day, day_str.c_str());
   // Show the month.
   lv_label_set_text(objects.lbl_month, m);
   // Show the year.
   lv_label_set_text(objects.lbl_year, String(y).c_str());
   // Show the day of the week.
   lv_label_set_text(objects.lbl_week_day, wd);
}

void GUI::showInitInfo(void)
{
   lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(init_color), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_label_set_text(objects.lbl_init_status, "GPIOs initialized.");
   delay_lvgl_ui(500);
   lv_label_set_text(objects.lbl_init_status, "Display initialized.");
   delay_lvgl_ui(500);
   lv_label_set_text(objects.lbl_init_status, "Connecting to WiFi...");
   delay_lvgl_ui(500);
}

void GUI::showSensorInfo(bool ok, const char* name)
{
   String str_stat = "";
   if (ok == true)
   {
      str_stat = String(name) + " initialized.";
      lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(init_color), LV_PART_MAIN | LV_STATE_DEFAULT);
   }
   else
   {
      str_stat = "Can't init " + String(name) + "!\nRestarting...";
      lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(fail_color), LV_PART_MAIN | LV_STATE_DEFAULT);
   }
   // Show on display.      
   lv_label_set_text(objects.lbl_init_status, str_stat.c_str());
   delay_lvgl_ui(300);
}

void GUI::showRTCInfo(bool ok)
{
   String str_stat = "";
   if (ok == true)
   {
      str_stat = "RTC initialized.";
      lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(init_color), LV_PART_MAIN | LV_STATE_DEFAULT);
   }
   else
   {
      str_stat = "Can't init RTC!\nRestarting...";
      lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(fail_color), LV_PART_MAIN | LV_STATE_DEFAULT);
   }
   // Show on display.      
   lv_label_set_text(objects.lbl_init_status, str_stat.c_str());
   delay_lvgl_ui(300);
}

void GUI::showCalibInfo(const char* name)
{
   String str_stat = "Calibrations read.\nNode is <" + String(name) + ">";
   lv_obj_set_style_text_color(objects.lbl_init_status, lv_color_hex(LV_COLOR_DARKGREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
   // Show on display.      
   lv_label_set_text(objects.lbl_init_status, str_stat.c_str());
   delay_lvgl_ui(300);
}
