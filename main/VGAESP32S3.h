/*
    Test for making libraries for Arduino
*/
#ifndef VGAESP32S3_h
#define VGAESP32S3_h
#include "VGARes.h"
#include "VGADisplayManager.h"

class VGAESP32S3
{
    public:
        void init(VGARes VGAResMode, VGADisplayManager VGADisp);
        esp_lcd_panel_handle_t panel_handle;
        esp_lcd_panel_handle_t panel_config;
        uint16_t *pixels;
        uint8_t *pixels8b;
    private:
        void *framebuffer;
};

#endif