/*
    Test for making libraries for Arduino
*/

#include "Arduino.h"
#include "VGARes.h"

class VGAESP32S3
{
    public:
        esp_lcd_panel_handle_t panel_handle;
        esp_lcd_panel_handle_t panel_config;
        uint16_t *pixels;
        uint8_t *pixels8b;
}

