#include <Arduino.h>
#include <soc/soc.h>
#if (WDT_ENABLED == 1)
#include <esp_task_wdt.h>
#endif
#include <soc/lp_aon_reg.h>
#include "FS.h"
#include <LittleFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#if (AHTXX == 1)
#include <Adafruit_AHTX0.h>
#endif
#if (SHT3X == 1)
#include <Adafruit_SHT31.h>
#endif
#include "gui.h"
#include "radioNet_cfg.h"

// Debug message macros
#if (LOG_ENABLED == 1)
#define LogInfo(...) SERIAL_PORT.printf(__VA_ARGS__)
#else
#define LogInfo(...)
#endif

// Default radio node index
#define DEFAULT_NODE_ID RADIO_NODE0_ID

// Temperature sensor definitions
#if (AHTXX == 1)
#define AHTX0_I2C_ADDR           0x38u
#endif
#if (SHT3X == 1)
#define SHT31_I2C_ADDR           0x44u
#endif
// Status LED
#define DO_LED_STAT LED_BUILTIN
// BOOT button
#define DI_BTN_BOOT 9
// TRIAC gate pin
#define DO_TRIAC_GATE 0
// ZCV detection pin
#define DI_ZCV_SYNCH 1
// Light switch pin
#define DI_SWITCH_LIGHT 2
// I2C bus
#define I2C_SDA                  22
#define I2C_SCL                  23
// Timeout for WiFi connection process
#define WIFI_CONN_TIMEDOUT    30u   //30 seconds

// Default times in seconds for morning turn ON/OFF and evening turn ON/OFF
#define TIME1_TURN_ON_SEC (5 * 3600 + 30 * 60)   // 05:30 hours
#define TIME1_TURN_OFF_SEC (7 * 3600)            // 07:00 hours
#define TIME2_TURN_ON_SEC (17 * 3600)            // 17:00 hours
#define TIME2_TURN_OFF_SEC (19 * 3600 + 30 * 60) // 19:30 hours
#define TIMEX_TURN_INVALID (362340)              // 99:99 hours

// Maximum and minimum brightness
#define BRIGHT_DIMM_MIN (5)   // 5%
#define BRIGHT_DIMM_MAX (95)  // 95%
#define BRIGHT_FULL_OFF (1)   // 1%
#define BRIGHT_FULL_ON  (98)  // 98%
// Incrementing and decrementing time ticks for brightness in percents
#define BRIGHT_INC_STEP (20) // 20%
#define BRIGHT_DEC_STEP (1)  // 1%

// Watchdog timeout in seconds
#define WDT_TIMEOUT 5u

// Light state data
typedef enum te_light_state
{
   LIGHT_STATE_UNINT = 0,
   LIGHT_STATE_TURNED_OFF = 1,
   LIGHT_STATE_TURNING_OFF = 2,
   LIGHT_STATE_TURNED_ON = 3,
   LIGHT_STATE_TURNING_ON = 4,
} TE_LightState;

// Node data object
typedef struct ts_node_data
{
   uint8_t nodeID;
   double temperature;
   double humidity;
   uint16_t pressure;
   uint16_t voc;
   double battery;
} TS_NodeData;

// Nodes data array
TS_NodeData nodesData[RADIO_NR_NODES];

#if (AHTXX == 1)
// AHTxx sensor object
Adafruit_AHTX0 *ahtX0Sens = NULL;
#endif
#if (SHT3X == 1)
// SHT31 sensor object
Adafruit_SHT31 *sht31Sens = NULL;
#endif

// TFT display
LGFX tft;


// Local Node ID read from file
uint8_t localNodeID = (uint8_t)DEFAULT_NODE_ID;

// Temperature, humidity values.
float temperature = 0.0;
float humidity = 0.0;
// Pressure, VOC remains 0, it is not measured by the node.
uint16_t pressure = 0u;
uint16_t voc = 0u;
// Battery voltage is not measured.
float batVolt = 0.0;

// Current time as object and in seconds
tm currentTime;
int32_t curr_seconds = -1;
int32_t prev_seconds = -1;

// Time control data
int32_t time1_on_sec[12] =
    {
        TIMEX_TURN_INVALID, // Jan
        TIME1_TURN_ON_SEC,  // Feb
        TIME1_TURN_ON_SEC,  // Mar
        TIME1_TURN_ON_SEC,  // Apr
        TIMEX_TURN_INVALID, // May
        TIMEX_TURN_INVALID, // Jun
        TIMEX_TURN_INVALID, // Jul
        TIMEX_TURN_INVALID, // Aug
        TIMEX_TURN_INVALID, // Sep
        TIMEX_TURN_INVALID, // Oct
        TIMEX_TURN_INVALID, // Nov
        TIMEX_TURN_INVALID  // Dec
};
int32_t time1_off_sec[12] =
    {
        TIMEX_TURN_INVALID,
        TIME1_TURN_OFF_SEC,
        TIME1_TURN_OFF_SEC,
        TIME1_TURN_OFF_SEC,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID
};
int32_t time2_on_sec[12] =
    {
        TIME2_TURN_ON_SEC,
        TIME2_TURN_ON_SEC,
        TIME2_TURN_ON_SEC,
        TIME2_TURN_ON_SEC,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID
};
int32_t time2_off_sec[12] =
    {
        TIME2_TURN_OFF_SEC,
        TIME2_TURN_OFF_SEC,
        TIME2_TURN_OFF_SEC,
        TIME2_TURN_OFF_SEC,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID,
        TIMEX_TURN_INVALID
};

// Dimmable light timer
hw_timer_t *dimmTimer = NULL;
// Light brightness in percents
int16_t light_brightness = 0;
// Array to map each procent of brightness to a delay in us to fire the triac
uint16_t fireTime_us[100] =
    {
        0, 638, 903, 1108, 1282, 1436, 1575, 1705, 1826, 1940, 2048,
        2152, 2252, 2348, 2441, 2532, 2620, 2706, 2789, 2871, 2952,
        3031, 3108, 3184, 3259, 3333, 3406, 3478, 3550, 3620, 3690,
        3759, 3828, 3896, 3963, 4030, 4097, 4163, 4229, 4294, 4359,
        4424, 4489, 4553, 4617, 4681, 4745, 4809, 4873, 4936, 5000,
        5064, 5127, 5191, 5255, 5319, 5383, 5447, 5511, 5576, 5641,
        5706, 5771, 5837, 5903, 5970, 6037, 6104, 6172, 6241, 6310,
        6380, 6450, 6522, 6594, 6667, 6741, 6816, 6892, 6969, 7048,
        7129, 7211, 7294, 7380, 7468, 7559, 7652, 7748, 7848, 7952,
        8060, 8174, 8295, 8425, 8564, 8718, 8892, 9097, 9362 //, 10000
};

// Light state object
TE_LightState light_state = LIGHT_STATE_UNINT;
const String states_str[] = {"Uninit", "Turned OFF", "Turning OFF", "Turned ON", "Turning ON"};

// Light switch status (pressed = true, released = false)
bool switch_toggled = false;
// Variables used to debounce the switch button.
uint8_t currBtnVal = HIGH;
uint8_t prevBtnVal = HIGH;

// WiFi creditentials.
String wifi_ssid = "default";
String wifi_pass = "default";
IPAddress wifi_ip;
// POST request URL and API Key
String post_url = "http://placeholder.com/post_data.php";
String api_key = "placeholder_key";
// GET data
String get_data;
JsonDocument jsondoc;
// POST data
String post_data;

// Date and time data
const char *ntpServer = "ro.pool.ntp.org";
const char* timezone = "EET-2EEST,M3.5.0/3,M10.5.0/4";
// Days of week.
//const char daysRo[7][4] = {"DUM", "LUN", "MAR", "MIE", "JOI", "VIN", "SAM"};
const char daysEn[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
// Months of the year.
//const char monthsRo[12][4] = {"IAN", "FEB", "MAR", "APR", "MAI", "IUN", "IUL", "AUG", "SEP", "OCT", "NOI", "DEC"};
const char monthsEn[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
// NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// ESP32T internal rtc
ESP32Time rtc;

// Serial port request.
String serialReq = "";

// Tasks handlers
TaskHandle_t task10msHandle = NULL;
TaskHandle_t task1sHandle = NULL;
TaskHandle_t task60sHandle = NULL;
// Synchronization mutex
SemaphoreHandle_t mutexTaskSync;

// Timing stuff.
uint32_t onehour_cnt = 0u;

// Status LED state.
boolean ledState = false;

// RTC synchro flag
boolean rtcSynchronized = false;

// WiFI offline mode
boolean wifiConnected = false;
// WiFi connection retry counter
uint8_t wifiRetry = 0u;

void setupIOPins();
void setupDisplay();
void setupWiFi();
void setupSensors();
void setupDimmTimer();
#if LV_USE_LOG != 0
void lvgl_print_log(lv_log_level_t level, const char *buf);
#endif
void task10ms(void *param);
void task10ms_init();
void task10ms_run();
void task1s(void *param);
void task1s_init();
void task1s_run();
void task60s(void *param);
void task60s_init();
void task60s_run();
void checkSwitchBtn();
void handleTime1Time2Events();
void readCalibrations();
void readAutoTimeValues();
void readTempHumid();
void preparePOSTData();
void sendPOSTReq();
void printNodeDataOnSerial();
void printTaskBegin(TickType_t osTick, const char *txt);
void printTaskEnd(TickType_t osTick, const char *txt);
void synchronizeRTC();
tm createDateTimeJSON(const char *str);
void showLclNodeData();
void showCtrlInput();
void showCtrlOutput();
void resetToBootloader();
boolean isReprogRequested();
boolean isResetRequested();
String readSerialRequest();
void controlLight();
void printCtrlInputOnSerial();
void printCtrlOutputOnSerial();
String getValueFromLine(String line);
void fireTriac();
void handleWiFiConnection();
void handleRTC();

void IRAM_ATTR zeroCrossISR()
{
   timerRestart(dimmTimer);
   // Set timer delay in microseconds.
   timerAlarm(dimmTimer, fireTime_us[(100 - light_brightness)], false, 0);
}

// Timer Interrupt to Fire the Triac
void IRAM_ATTR dimmTimerISR()
{
   fireTriac();
}

void setup()
{
   // Setup serial port.
   SERIAL_PORT.begin(115200);
#if (TEST_ENABLED == 1) // Enabled this just for testing purposes!
   while (SERIAL_PORT.available() <= 0)
   {
      LogInfo("Send any character via serial port to start...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
#endif
   LogInfo("Starting AC Light Control %s...\n\n", SW_VERSION);

   // Initialize I1C bus.
   Wire.setPins(I2C_SDA, I2C_SCL);
   // Setup digital IO pins.
   setupIOPins();

   // Setup display.
   setupDisplay();

   // Initialize file system.
   if (LittleFS.begin() == false)
   {
      // Restart the micrcontrolloer if failed to initialize.
      LogInfo("LittleFS Mount Failed! Restarting...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
   }

   // Read calibrations from calib.txt.
   readCalibrations();
   // Read time moments for automatic turn on/off from times.txt.
   readAutoTimeValues();

   // Setup WiFi network.
   setupWiFi();

   // Setup sensors.
   setupSensors();

   LogInfo("Node is %s.\n", nodesNamesRo[localNodeID]);

   // Make a few dummy reading of temperature and humidity. It seems that the first value is never read correctly.
   readTempHumid();
   readTempHumid();
   readTempHumid();
   vTaskDelay(pdMS_TO_TICKS(100));

   // Show main window.
   gui.showMainWnd();

   // Set light state to turned OFF.
   light_state = LIGHT_STATE_TURNED_OFF;

   // Initialize dimmable timer.
   setupDimmTimer();
   // Attach Zero-Cross Interrupt
   attachInterrupt(digitalPinToInterrupt(DI_ZCV_SYNCH), zeroCrossISR, RISING);

   // Read initial value of the switch button.
   prevBtnVal = digitalRead(DI_SWITCH_LIGHT);
   
#if (WDT_ENABLED == 1)
   // Enable task watchdog.    
   esp_task_wdt_config_t twdt_config = 
   {
        .timeout_ms = WDT_TIMEOUT * 1000u,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,    // Bitmask of all cores
        .trigger_panic = true,
    };
   esp_task_wdt_deinit();
   esp_task_wdt_init(&twdt_config);
   enableLoopWDT();
#endif

   // Create syncronization mutex.
   mutexTaskSync = xSemaphoreCreateMutex();
   if (mutexTaskSync == NULL)
   {
      LogInfo("Can't create task syncrhonization mutex! Resetting...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
   }
   xSemaphoreGive(mutexTaskSync);
   // Create cyclic tasks.
   xTaskCreate(task10ms, "task10ms", (4*1024u), NULL, (tskIDLE_PRIORITY + 1u), &task10msHandle);
   xTaskCreate(task1s, "task1s", (4*1024u), NULL, (tskIDLE_PRIORITY + 1u), &task1sHandle);
   xTaskCreate(task60s, "task60s", (4*1024u), NULL, (tskIDLE_PRIORITY + 1u), &task60sHandle);
#if (WDT_ENABLED == 1)
   // Add tasks to watchdog.
   esp_task_wdt_add(task10msHandle);
#endif
}

void loop()
{
   // Do nothing, everything is run by FreeRTOS scheduler.
   vTaskDelay(5);
}

void setupIOPins()
{
   LogInfo("Configuring IO pins...\n");
   // Initialize status LED.
   pinMode(DO_LED_STAT, OUTPUT);
   digitalWrite(DO_LED_STAT, LOW);
   // Initialize BOOT button.
   pinMode(DI_BTN_BOOT, INPUT_PULLUP);
   // Initialize TRIAC gate pin.
   pinMode(DO_TRIAC_GATE, OUTPUT);
   digitalWrite(DO_TRIAC_GATE, LOW);
   // Initialize ZCV pin.
   pinMode(DI_ZCV_SYNCH, INPUT);
   // Initialize light switch pin.
   pinMode(DI_SWITCH_LIGHT, INPUT_PULLUP);
#if (EXTERNAL_ANTENNA == 1)
   // Activate RF switch control.
   pinMode(WIFI_ENABLE, OUTPUT);
   digitalWrite(WIFI_ENABLE, LOW);
   delay(100);
   // Use external antenna.
   pinMode(WIFI_ANT_CONFIG, OUTPUT);
   digitalWrite(WIFI_ANT_CONFIG, HIGH);
#endif

   LogInfo("IO pins initialized.\n");
}

void setupDisplay()
{
   char title[18];
   sprintf(title, "LightControl %s", SW_VERSION);

#if LV_USE_LOG != 0
   lv_log_register_print_cb(lvgl_print_log);
#endif
   LogInfo("Initializing the display...\n");
   // Initialize the display with LovyanGFX.
   tft.init();
   tft.setRotation(0);     // Set to landscape mode
   tft.setSwapBytes(true);
   LogInfo("Creating the GUI...\n");
   // Initialize the GUI.
   gui.begin(&tft);
   // Show initial screen.
   gui.showInitScreen(title);
   // Show first init info on display.
   gui.showInitInfo();
   LogInfo("Display and GUI initialized.\n");
}

void setupWiFi()
{
   // Start connecting.
   WiFi.begin(wifi_ssid, wifi_pass);
   LogInfo("Start connecting to %s...\n", wifi_ssid.c_str());
   // Initialize WiFi retry counter.
   wifiRetry = 0u;
}

void setupSensors()
{   
#if (AHTXX == 1)
   // Initialize AHTX0 object.
   if (ahtX0Sens == NULL)
   {
      ahtX0Sens = new Adafruit_AHTX0();
   }
   // Initialize AHTXX sensor.
   if (ahtX0Sens->begin() == false)
   {
      LogInfo("Could not initialize AHTX0 sensor!\n");
   }
   else
   {
      LogInfo("AHTX0 sensor initialized.\n");
   }
#endif
#if (SHT3X == 1)
   // Initialize SHT31 object.
   if (sht31Sens == NULL)
   {
      sht31Sens = new Adafruit_SHT31();
   }
   // Initialize SHT31 sensor.
   if (sht31Sens->begin() == false)
   {
      LogInfo("Could not initialize SHT31 sensor!\n");
   }
   else
   {
      LogInfo("SHT31 sensor initialized.\n");
   }
#endif
}

void setupDimmTimer()
{
   // Configure the timer used for dimming.
   dimmTimer = timerBegin(1000000u); //  tick = 1us
   if (dimmTimer != NULL)
   {
      timerAttachInterrupt(dimmTimer, &dimmTimerISR);
   }
   else
   {
      LogInfo("Can't initialize dimming timer! Resetting...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
   }
}

#if LV_USE_LOG != 0
void lvgl_print_log(lv_log_level_t level, const char *buf)
{
   LV_UNUSED(level);
    DebugPrintf("%s", buf);
   SERIAL_PORT.flush();
}
#endif

void task10ms(void *param)
{
   TickType_t osTickBegin, osTickEnd;

   // Initialize the task.
   task10ms_init();
   vTaskDelay(pdMS_TO_TICKS(50));
   // Run infinite loop.
   while (1)
   {
#if (WDT_ENABLED == 1)
      // Feed the task watchdog.
      esp_task_wdt_reset();
#endif
      // Check the synchronization mutex.
      if (xSemaphoreTake(mutexTaskSync, 0) == pdTRUE)
      {
         // Get current os tick.
         osTickBegin = xTaskGetTickCount();
         // Execute the task function.
         //printTaskBegin(osTickBegin, "task10ms");
         task10ms_run();
         // Get current os tick.
         osTickEnd = xTaskGetTickCount();
         //printTaskEnd(osTickEnd, "task10ms");
         // Release the synchronization mutex for other tasks.
         xSemaphoreGive(mutexTaskSync);
         // Delay until 10 ms.
         xTaskDelayUntil(&osTickBegin, pdMS_TO_TICKS(10));
      }
      else
      {
         // Just yeld control to the scheduler for 1 tick.
         vTaskDelay(1);
      }
   } 
}

void task10ms_init()
{
   LogInfo("task10ms initialized.\n");
}

void task10ms_run()
{
   // Run LVGL task.
   lv_task_handler();
   lv_tick_inc(10);

   // Check if a request is received via Serial port.
   serialReq = readSerialRequest();

   // Check if reprogramming is requested.
   if (isReprogRequested() == true)
   {
      // Reset to bootloader.
      resetToBootloader();
   }

   // Check if reset is requested.
   if (isResetRequested() == true)
   {
      // Reset.
      SERIAL_PORT.println("\tResetting...");
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
   }
}

void task1s(void *param)
{
   TickType_t osTickBegin, osTickEnd;

   // Initialize the task.
   task1s_init();
   vTaskDelay(pdMS_TO_TICKS(70));
   // Run infinite loop.
   while (1)
   {
      // Check the synchronization mutex.
      if (xSemaphoreTake(mutexTaskSync, 0) == pdTRUE)
      {
         // Get current os tick.
         osTickBegin = xTaskGetTickCount();
         // Execute the task function.
         printTaskBegin(osTickBegin, "task1s");
         task1s_run();
         osTickEnd = xTaskGetTickCount();
         printTaskEnd(osTickEnd, "task1s");
         // Release the synchronization mutex for other tasks.
         xSemaphoreGive(mutexTaskSync);
         // Delay until 1 s.
         xTaskDelayUntil(&osTickBegin, pdMS_TO_TICKS(1000));
      }
      else
      {
         // Just yeld control to the scheduler for 1 tick.
         vTaskDelay(1);
      }
   }        
}

void task1s_init()
{
   LogInfo("task1s initialized.\n");
}

void task1s_run()
{
   // Toggle status LED.
   ledState = !ledState;
   gui.showStatusLED(ledState);
   digitalWrite(DO_LED_STAT, ledState);

   // Check switch button state.
   checkSwitchBtn();

   // Check and handle WiFi connection/reconnection.
   handleWiFiConnection();

   // Handle RTC.
   handleRTC();

   // Print inputs on serial.
   printCtrlInputOnSerial();
   // Show inputs on display.
   showCtrlInput();
   // Control the light bulb.
   controlLight();
   // Print on serial.
   printCtrlOutputOnSerial();
   // Show on display.
   showCtrlOutput();
}

void task60s(void *param)
{
   TickType_t osTickBegin, osTickEnd;

   // Initialize the task.
   task60s_init();
   vTaskDelay(pdMS_TO_TICKS(90));
   // Run infinite loop.
   while (1)
   {
      // Check the synchronization mutex.
      if (xSemaphoreTake(mutexTaskSync, 0) == pdTRUE)
      {
         // Get current os tick.
         osTickBegin = xTaskGetTickCount();
         // Execute the task function.
         printTaskBegin(osTickBegin, "task60s");
         task60s_run();
         osTickEnd = xTaskGetTickCount();
         printTaskEnd(osTickEnd, "task60s");
         // Release the synchronization mutex for other tasks.
         xSemaphoreGive(mutexTaskSync);
         // Delay until 60 s.
         xTaskDelayUntil(&osTickBegin, pdMS_TO_TICKS(60000));
      }
      else
      {
         // Just yeld control to the scheduler for 1 tick.
         vTaskDelay(1);
      }
   }  
}

void task60s_init()
{
   LogInfo("task60s initialized.\n");
}

void task60s_run()
{   
   // Read local temperature and humidity.
   readTempHumid();
   // Prepare for display.
   nodesData[localNodeID].temperature = temperature;
   nodesData[localNodeID].humidity = humidity;
   // Show data on display.
   showLclNodeData();
   // Print local node information on serial port.
   printNodeDataOnSerial();

   if (wifiConnected == true)
   {
      // Prepare to POST local data to webserver.
      preparePOSTData();
      // POST local data to webserver.
      sendPOSTReq();
   }

   // Increment and check one hour counter.
   onehour_cnt = (onehour_cnt + 1) % 60u;
   if (onehour_cnt == 0u)
   {
      // Resinchronize internal RTC every 1 hour(60 minutes).
      synchronizeRTC();
   }
}

void checkSwitchBtn()
{
   // Read the state of the switch button.
   currBtnVal = digitalRead(DI_SWITCH_LIGHT);
   // Check if it is a raising or falling edge.
   if (prevBtnVal != currBtnVal)
   {
      // Set switch state.
      switch_toggled = true;
      LogInfo("\tSwitch toggled.\n");
   }

   // Store current value.
   prevBtnVal = currBtnVal;
}

void handleTime1Time2Events()
{
   // Simulate a fake switch toggle when time reaches time1 ON limit.
   if ((prev_seconds < time1_on_sec[currentTime.tm_mon]) && (curr_seconds >= time1_on_sec[currentTime.tm_mon]) && (light_state == LIGHT_STATE_TURNED_OFF))
   {
      // Force switch toggle.
      switch_toggled = true;
      LogInfo("\tFAKE switch toggle due to time1_on moment.\n");
   }
   // Simulate a fake switch toggle when time reaches time1 OFF limit.
   else if ((prev_seconds < time1_off_sec[currentTime.tm_mon]) && (curr_seconds >= time1_off_sec[currentTime.tm_mon]) && (light_state == LIGHT_STATE_TURNED_ON))
   {
      // Force switch toggle.
      switch_toggled = true;
      LogInfo("\tFAKE switch toggle due to time1_off moment.\n");
   }
   // Simulate a fake switch toggle when time reaches time2 ON limit.
   else if ((prev_seconds < time2_on_sec[currentTime.tm_mon]) && (curr_seconds >= time2_on_sec[currentTime.tm_mon]) && (light_state == LIGHT_STATE_TURNED_OFF))
   {
      // Force switch toggle.
      switch_toggled = true;
      LogInfo("\tFAKE switch toggle due to time2_on moment.\n");
   }
   // Simulate a fake switch toggle when time reaches time2 OFF limit.
   else if ((prev_seconds < time2_off_sec[currentTime.tm_mon]) && (curr_seconds >= time2_off_sec[currentTime.tm_mon]) && (light_state == LIGHT_STATE_TURNED_ON))
   {
      // Force switch toggle.
      switch_toggled = true;
      LogInfo("\tFAKE switch toggle due to time2_off moment.\n");
   }
   else
   {
      // Do nothing.
   }
}

void readCalibrations()
{
   String line;

   LogInfo("Reading file calib.txt...\n");

   File file = LittleFS.open("/calib.txt");
   if (!file)
   {
      LogInfo("Failed to open file for reading! Using default values.\n");
   }
   else
   {
      LogInfo("\tReading calibrations from file...\n");
      while (file.available())
      {
         // Read the lines in the file.
         line = file.readStringUntil('\n');
         line.trim();
         // Skip lines that start with ';' char.
         if (line.startsWith(";") == false)
         {
            // Check for node ID.
            if (line.startsWith("nodeid") == true)
            {
               localNodeID = (uint8_t)getValueFromLine(line).toInt();
            }
            // Check for WiFi ssid.
            if (line.startsWith("wifi_ssid") == true)
            {
               wifi_ssid = getValueFromLine(line);
            }
            // Check for WiFi password.
            if (line.startsWith("wifi_pass") == true)
            {
               wifi_pass = getValueFromLine(line);
            }
            // Check for POST URL.
            if (line.startsWith("post_url") == true)
            {
               post_url = getValueFromLine(line);
            }
            // Check for API Key.
            if (line.startsWith("api_key") == true)
            {
               api_key = getValueFromLine(line);
            }
         }
      }
   }
   file.close();

   LogInfo("NodeID is %d.\n", localNodeID);
   LogInfo("WiFi ssid is %s.\n", wifi_ssid.c_str());
   LogInfo("POST URL is %s.\n", post_url.c_str());
}

void readAutoTimeValues()
{
   String line;
   uint8_t month = 0u;
   int8_t hrs = 99;
   int8_t min = 99;

   LogInfo("Reading file times.txt...\n");

   File file = LittleFS.open("/times.txt");
   if (!file)
   {
      LogInfo("Failed to open file for reading! Using default values.\n");
   }
   else
   {
      LogInfo("\tReading time values from file...\n");
      while (file.available())
      {
         // Read the lines in the file.
         line = file.readStringUntil('\n');
         line.trim();
         // Skip lines that start with ';' char.
         if (line.startsWith(";") == false)
         {
            // Read the hour value for time1_on moment.
            hrs = (int8_t)line.substring(0, 2).toInt();
            // Read the minute value for time1_on moment.
            min = (int8_t)line.substring(3, 5).toInt();
            // Compute time1_on in seconds.
            time1_on_sec[month] = hrs * 3600 + min * 60;
            LogInfo(" time1_on[%d] = %d:%d %dsec ", month, hrs, min, time1_on_sec[month]);
            // Read the hour value for time1_off moment.
            hrs = (int8_t)line.substring(6, 8).toInt();
            // Read the minute value for time1_off moment.
            min = (int8_t)line.substring(9, 11).toInt();
            // Compute time1_off in seconds.
            time1_off_sec[month] = hrs * 3600 + min * 60;
            LogInfo(" time1_off[%d] = %d:%d %dsec ", month, hrs, min, time1_off_sec[month]);
            // Read the hour value for time2_on moment.
            hrs = (int8_t)line.substring(12, 14).toInt();
            // Read the minute value for time2_on moment.
            min = (int8_t)line.substring(15, 17).toInt();
            // Compute time2_on in seconds.
            time2_on_sec[month] = hrs * 3600 + min * 60;
            LogInfo(" time2_on[%d] = %d:%d %dsec ", month, hrs, min, time2_on_sec[month]);
            // Read the hour value for time2_off moment.
            hrs = (int8_t)line.substring(18, 20).toInt();
            // Read the minute value for time2_off moment.
            min = (int8_t)line.substring(21, 23).toInt();
            // Compute time2_off in seconds.
            time2_off_sec[month] = hrs * 3600 + min * 60;
            LogInfo(" time2_off[%d] = %d:%d %dsec\n", month, hrs, min, time2_off_sec[month]);
            // Increment the month index.
            month++;
         }
         else
         {
            // Just print the month name.
            LogInfo("\t\t%10s:", line.substring(1));
         }
      }
   }
   file.close();
}

void readTempHumid()
{
   LogInfo("Reading temperature and humidity...\n");
#if (AHTXX == 1)
   sensors_event_t humi, temp;
   // Populate temp and humid values with fresh data.
   if (ahtX0Sens->getEvent(&humi, &temp) == true)
   {
      temperature = temp.temperature;
      humidity = humi.relative_humidity;
   }
#endif
#if (SHT3X == 1)
   // Populate temp and humid values with fresh data.
   sht31Sens->readBoth(&temperature, &humidity);
#endif
   LogInfo("Done.\n");
}

void preparePOSTData()
{
   post_data = "{\"api_key\":\"" + api_key + "\",";
   post_data += ("\"id\":\"" + String(localNodeID) + "\",");
   post_data += ("\"temperature\":\"" + String(temperature, 1) + "\",");
   post_data += ("\"humidity\":\"" + String(humidity, 1) + "\",");
   post_data += ("\"pressure\":\"" + String(pressure) + "\",");
   post_data += ("\"tvoc\":\"" + String(voc) + "\",");
   post_data += ("\"eco2\":\"" + String(0) + "\",");
   post_data += ("\"battery\":\"" + String(batVolt, 2) + "\"}");

   LogInfo("POST data is: %s\n", post_data.c_str());
}

void sendPOSTReq()
{
   HTTPClient http;

   // Start HTTP client.
   http.begin(post_url);

   http.addHeader("Content-Type", "application/json");

   LogInfo("Sending HTTP POST request...\n");
   // Send HTTP request.
   int httpResponseCode = http.POST(post_data);

   if (httpResponseCode > 0)
   {
      LogInfo("HTTP Response code: %d\n", httpResponseCode);
      // Get the content of the response.
      String payload = http.getString();
      LogInfo("%s\n", payload.c_str());
   }
   else
   {
      LogInfo("Error code: %d\n", httpResponseCode);
   }
   http.end();
}

void printNodeDataOnSerial()
{
   // Send via serial comm.
   LogInfo("Local Node ID: %d\n", localNodeID);
   LogInfo("Node: %s\n", nodesNamesRo[localNodeID]);
   LogInfo("Temperature: %f.1 degC\n", temperature);
   LogInfo("Humidity: %f.1 %rH\n", humidity);
   LogInfo("Pressure: %d mmHg\n", pressure);
   LogInfo("VOC index: %d\n", voc);
   LogInfo("Battery: %f.2 V\n\n", batVolt);
}

void printTaskBegin(TickType_t osTick, const char *txt)
{
   // Print timing information.
   LogInfo("%d : BEGIN TASK %s\n", pdTICKS_TO_MS(osTick), txt);
}

void printTaskEnd(TickType_t osTick, const char *txt)
{
   // Print timing information.
   LogInfo("%d : END TASK %s\n", pdTICKS_TO_MS(osTick), txt);
}

void synchronizeRTC()
{
   time_t rawTime;

   LogInfo("\tGetting current date and time from %s...\n", ntpServer);
   // Synchronize with NTP server.
   if (timeClient.update() == true)
   {
      // Parse the response from NTP server.
      rawTime = timeClient.getEpochTime();
      LogInfo("\tResponse from %s: %d\n", ntpServer, rawTime);
      // Set internal RTC time.
      setenv("TZ", timezone, 1);            // Set environment variable with your time zone
      tzset();
      rtc.setTime(rawTime);
      // Read current time.
      currentTime = rtc.getTimeStruct();
      LogInfo("\tCurrent time is %s", daysEn[currentTime.tm_wday]);
      LogInfo(" %02d.%02d.%d", currentTime.tm_mday, (currentTime.tm_mon + 1), (currentTime.tm_year + 1900));
      LogInfo(" %02d:%02d:%02d\n", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
      // Calculate current seconds of the day.
      curr_seconds = currentTime.tm_hour * 3600 + currentTime.tm_min * 60 + currentTime.tm_sec;
      prev_seconds = curr_seconds - 1;
      LogInfo("\tCurrent second in day is %d. ", curr_seconds);
      LogInfo("\tPrevious second is %d.\n", prev_seconds);

      // Set synchronized flag.
      rtcSynchronized = true;
   }
   else
   {
      LogInfo("\tFailed.\n");
   }
   //Update GUI.
   gui.showRTCInfo(true);
}

tm createDateTimeJSON(const char *str)
{
   tm dt;
   tm dt2;
   int year, month, day, hour, minute, second;

   // Find the time and date.
   sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
   dt.tm_year = year - 1900;
   dt.tm_mon = month - 1;
   dt.tm_mday = day;
   dt.tm_hour = hour;
   dt.tm_min = minute;
   dt.tm_sec = second;
   dt2.tm_year = year - 1900;
   dt2.tm_mon = month - 1;
   dt2.tm_mday = day;
   dt2.tm_hour = hour;
   dt2.tm_min = minute;
   dt2.tm_sec = second;

   // Find the day of the week.
   time_t local_time = mktime(&dt2);
   dt.tm_wday = localtime(&local_time)->tm_wday;

   return dt;
}

void showLclNodeData()
{
   LogInfo("\tShow local node %d on display.\n", localNodeID);

   // Show temperature.
   gui.showTemperature(nodesData[localNodeID].temperature);

   // Show humidity value.
   gui.showHumidity((uint8_t)nodesData[localNodeID].humidity);
}

void showCtrlInput()
{
   // Show switch button state and web server request.
   gui.showSwitchState(switch_toggled);
}

void showCtrlOutput()
{
   // Show light state and brightness.
   gui.showLightState((uint8_t)light_state);
   gui.showLightBrightness(light_brightness);
}

void resetToBootloader()
{
   SERIAL_PORT.println("\tResetting to bootloader...");
   vTaskDelay(pdMS_TO_TICKS(1000));
   REG_WRITE(LP_AON_SYS_CFG_REG, LP_AON_FORCE_DOWNLOAD_BOOT);
   esp_restart();
}

boolean isReprogRequested()
{
   boolean ret = false;

   if ((serialReq.startsWith("d") == true) || (serialReq.startsWith("D") == true))
   {
      ret = true;
      SERIAL_PORT.println("\tReprogramming requested.");
   }

   return ret;
}

boolean isResetRequested()
{
   boolean ret = false;

   if ((serialReq.startsWith("r") == true) || (serialReq.startsWith("R") == true))
   {
      ret = true;
      SERIAL_PORT.println("\tReset requested.");
   }

   return ret;
}

String readSerialRequest()
{
   String s = "";

   if (SERIAL_PORT.available() > 0)
   {
      // Bytes are received, read them.
      s = SERIAL_PORT.readString();
   }
   return s;
}

void controlLight()
{
   static uint8_t state_cnt = 0u;

   // Execute light control state machine.
   switch (light_state)
   {
   case LIGHT_STATE_UNINT:
      break;
   case LIGHT_STATE_TURNED_OFF:
      // Ensure light is turned OFF fully.
      light_brightness = BRIGHT_FULL_OFF;

      // Check conditions to change the state to turning ON:
      // switch button is pressed
      if (switch_toggled == true)
      {
         // Go to Turning ON state.
         light_state = LIGHT_STATE_TURNING_ON;
      }
      break;
   case LIGHT_STATE_TURNING_OFF:
      // From 95% to 20%
      if (light_brightness > 20)
      {
         // Decrement brightness with 5% every 1 second => 16 seconds.
         light_brightness -= (5 * BRIGHT_DEC_STEP);
      }
      // From 20% to 10%
      else if (light_brightness > 10)
      {
         // Decrement brightness with 1% every 6 seconds => 60 seconds.
         state_cnt = (state_cnt + 1u) % 6u;
         if (state_cnt == 0u)
         {
            light_brightness -= BRIGHT_DEC_STEP;
         }
      }
      // From 10% to 6%
      else if (light_brightness > (BRIGHT_DIMM_MIN + 1))
      {
         // Decrement brightness with 1% every 7 seconds => 28 seconds.
         state_cnt = (state_cnt + 1u) % 7u;
         if (state_cnt == 0u)
         {
            light_brightness -= BRIGHT_DEC_STEP;
         }
      }
      else
      {
         // Decrement brightness with 1% every 30 seconds => 30 seconds.
         state_cnt = (state_cnt + 1u) % 30u;
         if (state_cnt == 0u)
         {
            light_brightness -= BRIGHT_DEC_STEP;
         }
      }
      // Saturate the brightness value.
      if (light_brightness <= BRIGHT_DIMM_MIN)
      {
         light_brightness = BRIGHT_DIMM_MIN;
      }

      // Check conditions to change the state to turned OFF:
      // dimming is finished and brightness is 0
      if (light_brightness == BRIGHT_DIMM_MIN)
      {
         // Go to Turned OFF state.
         light_state = LIGHT_STATE_TURNED_OFF;
      }

      // Check conditions to change the state to turning ON:
      // switch button is pressed
      if (switch_toggled == true)
      {
         // Go to Turning ON state.
         light_state = LIGHT_STATE_TURNING_ON;
      }
      break;
   case LIGHT_STATE_TURNED_ON:
      // Ensure light is turned ON fully.
      light_brightness = BRIGHT_FULL_ON;

      // Check conditions to change the state to turning OFF:
      // switch button is pressed
      if (switch_toggled == true)
      {
         // Go to Turning OFF state.
         light_state = LIGHT_STATE_TURNING_OFF;
      }
      break;
   case LIGHT_STATE_TURNING_ON:
      // Increment brightness.
      light_brightness += BRIGHT_INC_STEP;
      // Saturate the brightness value.
      if (light_brightness >= BRIGHT_DIMM_MAX)
      {
         light_brightness = BRIGHT_DIMM_MAX;
      }

      // Check conditions to change the state to turned ON:
      // dimming is finished and brightness is maximum
      if (light_brightness == BRIGHT_DIMM_MAX)
      {
         // Go to Turned ON state.
         light_state = LIGHT_STATE_TURNED_ON;
      }

      // Check conditions to change the state to turning OFF:
      // switch button is pressed
      if (switch_toggled == true)
      {
         // Go to Turning OFF state.
         light_state = LIGHT_STATE_TURNING_OFF;
      }
      break;
   default:
      break;
   }

   // Clear switch button status at the end of the control state machine. It will be set again when switch is pressed.
   switch_toggled = false;
}

void printCtrlInputOnSerial()
{
   // Send via serial comm.
   LogInfo("\tControl inputs:\n");
   LogInfo("\t\tSwitch button: %d\n", switch_toggled);
}

void printCtrlOutputOnSerial()
{
   // Send via serial comm.
   LogInfo("\tControl outputs:\n");
   LogInfo("\t\tLight state: %s\n", states_str[light_state].c_str());
   LogInfo("\t\tLight brightness: %d\n", light_brightness);
}

String getValueFromLine(String line)
{
   int delimPos = line.indexOf('=');
   return (line.substring(delimPos + 1));
}

void fireTriac()
{
   // Short pulse to trigger the triac.
   digitalWrite(DO_TRIAC_GATE, HIGH);
   delayMicroseconds(10);
   digitalWrite(DO_TRIAC_GATE, LOW);
}

void handleWiFiConnection()
{
   // Read WiFi status.
   wl_status_t status = WiFi.status();

   // First, check if WiFi is during connection.
   if ((wifiConnected == false) && (status != WL_CONNECTED))
   {
      //WiFi is still connecting, increment retry counter.
      wifiRetry++;
      LogInfo("\tWiFi status: %d. Retry: %d\n", (int)status, wifiRetry);
      // Reset the microcontroller if connecting process timed out.
      if (wifiRetry >= WIFI_CONN_TIMEDOUT)
      {
         LogInfo("\tWiFi connection timed out! Resetting...\n");
         vTaskDelay(pdMS_TO_TICKS(1000));
         esp_restart();
      }
   }
   // Check if WiFi has disconnected.
   if ((wifiConnected == true) && (status != WL_CONNECTED))
   {
      LogInfo("\tDisconnected from WiFi access point!\n");
      // Clear WiFi connection flag.
      wifiConnected = false;
      // Force reconnection.
      WiFi.disconnect();
      setupWiFi();      
   }
   // Check if WiFi has connected.
   if ((wifiConnected == false) && (status == WL_CONNECTED))
   {
      LogInfo("\tConnected to WiFi access point successfully!\n");
      LogInfo("\tIP address: %s\n", WiFi.localIP().toString());
      // Set the connection flag.
      wifiConnected = true;
   }
   // Update WiFi status on GUI.
   gui.showWiFiState(wifiConnected);
}

void handleRTC()
{
   // CHeck if RTC is already synchronized.
   if (rtcSynchronized == true)
   {
      // Read date and time.
      currentTime = rtc.getTimeStruct();
      LogInfo("\tCurrent time is %s", daysEn[currentTime.tm_wday]);
      LogInfo(" %02d.%02d.%d", currentTime.tm_mday, (currentTime.tm_mon + 1), (currentTime.tm_year + 1900));
      LogInfo(" %02d:%02d:%02d\n", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
      // Calculate current seconds of the day.
      prev_seconds = curr_seconds;
      curr_seconds = currentTime.tm_hour * 3600 + currentTime.tm_min * 60 + currentTime.tm_sec;
      LogInfo("\tCurrent second in day is %d. Previous second is %d.\n", curr_seconds, prev_seconds);
      // Show date and time.
      gui.showTime((uint8_t)currentTime.tm_hour, (uint8_t)currentTime.tm_min);
      gui.showDate((uint8_t)currentTime.tm_mday, monthsEn[currentTime.tm_mon], (uint16_t)(currentTime.tm_year + 1900), daysEn[currentTime.tm_wday]);

      // Check conditions to switch the light by time1 and time2 intervals.
      handleTime1Time2Events();
   }
   else  // RTC is not synchronized.
   {
      // Show invalid date and time.
      gui.showTime(99u, 99u);
      gui.showDate(99u, "MON", 9999u, "NO DATE!");
      // Check if WiFi is connected.
      if (wifiConnected == true)
      {
         // Initialize a NTPClient to get time.
         timeClient.begin();
         // Syncronize internal RTC with NTP server.
         synchronizeRTC();
         // Initialize one hour counter.
         onehour_cnt = 1u;
      }
   }
}
