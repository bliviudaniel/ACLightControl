#ifndef _GUI_H_
#define _GUI_H_
#include "colors.h"
#include "LGFX_ESP32_ST7735.h"
#include <lvgl.h>
#include "ui.h"

// Set to your screen resolution and rotation
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   160

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE ((SCREEN_WIDTH * SCREEN_HEIGHT * (LV_COLOR_DEPTH / 8)) / 10)

class GUI
{
   private:
   uint8_t              draw_buf[DRAW_BUF_SIZE];
   const uint32_t       init_color;
   const uint32_t       wifi_color;
   const uint32_t       fail_color;
   const uint32_t       on_color;
   const uint32_t       off_color;
   const lv_img_dsc_t*  light_icons[4];
   const lv_img_dsc_t*  wifi_icons[2];
   
   public:
   LGFX *tft;
   GUI();
   virtual ~GUI(){};
   void begin(LGFX *tft_disp);
   void delay_lvgl_ui(uint32_t ms);
   void showInitScreen(const char* title);
   void showMainWnd(void);
   void showTime(uint8_t hrs, uint8_t min);
   void showDate(uint8_t d, const char* m, uint16_t y, const char* wd);
   void showTemperature(double val);
   void showHumidity(uint8_t val);
   void showLightBrightness(uint8_t val);
   void showLightState(uint8_t state);
   void showSwitchState(bool state);
   void showWiFiState(bool state);
   void showStatusLED(bool state);
   void showInitInfo(void);
   void showSensorInfo(bool ok, const char* name);
   void showRTCInfo(bool ok);
   void showCalibInfo(const char* name);
};

extern GUI gui;

#endif //_GUI_H_