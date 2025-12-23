/*
    Test for making libraries for Arduino
*/
#ifndef VGAESP32S3_h
#define VGAESP32S3_h
#include "VGARes.h"

class VGAESP32S3
{
    public:
        void init(VGARes VGAResMode);
        esp_lcd_panel_handle_t panel_handle;
        esp_lcd_panel_handle_t panel_config;
        uint16_t *pixels;
        uint8_t *pixels8b;
    private:
        void *framebuffer;
};

#endif