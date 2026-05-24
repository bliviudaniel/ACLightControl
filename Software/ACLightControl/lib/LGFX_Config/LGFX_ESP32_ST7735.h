#pragma once

// This is a custom configuration for ESP32 + ST7735
// Define LGFX_USE_V1 in platformio.ini
#define LGFX_USE_V1

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
   // Provide panel parameters
   lgfx::Panel_ST7735S _panel_instance;

   // Provide bus parameters
   lgfx::Bus_SPI _bus_instance;

public:
   LGFX(void)
   {
      {                                     // Configure bus control settings
         auto cfg = _bus_instance.config(); // Get the bus configuration structure

         // SPI settings
         cfg.spi_host = SPI2_HOST;  // Select SPI2
         cfg.spi_mode = 0;          // SPI mode 0
         cfg.freq_write = 25000000; // SPI clock for writing (50 MHz)
         cfg.freq_read = 16000000;  // SPI clock for reading (16 MHz)
         cfg.pin_sclk = 19;          // SPI SCLK pin
         cfg.pin_mosi = 18;         // SPI MOSI pin
         cfg.pin_miso = -1;         // SPI MISO pin (-1 if not used)
         cfg.pin_dc = 20;            // Data/Command pin (D/C)
         cfg.dma_channel = SPI_DMA_DISABLED;

         _bus_instance.config(cfg);              // Apply the settings
         _panel_instance.setBus(&_bus_instance); // Set the bus to the panel
      }

      {                                       // Configure display panel control settings
         auto cfg = _panel_instance.config(); // Get the panel configuration structure

         cfg.pin_cs = 17;    // Chip select pin (CS)
         cfg.pin_rst = 16;   // Reset pin (RST)
         cfg.pin_busy = -1;  // Busy pin (-1 if not used)

         // Basic display settings
         cfg.panel_width = 128;
         cfg.panel_height = 160;
         cfg.offset_x = 0;
         cfg.offset_y = 0;
         cfg.offset_rotation = 0;
         cfg.dummy_read_pixel = 8;
         cfg.readable = true;
         cfg.invert = false;
         cfg.rgb_order = false;
         cfg.dlen_16bit = false;
         cfg.bus_shared = true;

         _panel_instance.config(cfg);
      }

      setPanel(&_panel_instance); // Set the panel to be used
   }
};