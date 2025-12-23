/*
    Test for making libraries for Arduino
*/

#include "Arduino.h"
#include "VGAres.h"

class VGAESP32S3
{
    private:
        esp_lcd_panel_handle_t panel_handle;
        esp_lcd_panel_handle_t panel_config;
}

