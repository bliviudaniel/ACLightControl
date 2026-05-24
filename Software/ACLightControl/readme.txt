About:
   This is the software for controlling 220V AC light bulb/dimmable LED. 
   Main features:
      1. When light is turned ON by switch, it will gradually increase the intensity in a configurable time (initially 10s).
      2. When light is shut OFF by switch, it will gradually decrease the intensity in a configurable time (initially 60s).
      3. Light is turned ON/OFF via 3 ways:
         a. at configurable fixed time moments
         b. when a switch button is pressed
         c. via HTTP GET request
      4. It will read temperature and humidity via HTTP GET request from webserver.
      5. Light state will be sent to webserver via HTTP POST request.
      6. Temperature, humidity and light state are diplayed on a 2.8" ILI9341 TFT display.


Version changelog:

0.1
   Initial version.
0.2
   Add TFT_eSPI library instead of Adafruit.
   Add the time to increase and decrease the brightness as calibrations.
0.3
   Remove dimmable light library.
   Use power based dimming by splitting the hals sine wave into 100 equal area parts.
1.0
   Limit brightness between 5% and 95% to avoid flickering.
1.1
   Update turning off timings.
   Avoid automatic turn on/off when light is already on/off.
   Implement daylight time adjustment.
2.0
   Use FreeRTOS tasks and Task Watchdog.
   Change WiFi connection to polling instead of events.
   Update logging functions.
3.0
   Add SHT30 sensors reading and reporting.
3.1
   Made changes in GUI.
4.0
   Replace TFT_eSPI with LovyanGFX.
Reset to 1.0 version at this point before uploading to Git.
