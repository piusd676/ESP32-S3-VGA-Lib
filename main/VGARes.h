#ifndef VGARes_h
#define VGARes_h

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

class VGARes {
    public:
        static VGARes R640x480at60hz16bit;
        static VGARes R640x480at60hz8bit;
        static VGARes R640x480at60hz4bit;
        static VGARes R640x480at60hz3bit;
        uint32_t pixelclockhz;
        uint32_t hres;
        uint32_t vres;
        uint32_t hsyncpulsewidth;
        uint32_t hsyncbackwidth;
        uint32_t hsyncfrontwidth;
        uint32_t vsyncpulsewidth;
        uint32_t vsyncbackwidth;
        uint32_t vsyncfrontwidth;
        uint32_t hsyncidlelow;
        uint32_t vsyncidlelow;
        uint32_t pclkactivenegative;
        size_t datawidth;
        size_t bitsperpixel;
        size_t num_frbfs;
        size_t bouncebfrsize;
        size_t sramtransalign;
        size_t psramtransalign;
        int hsyncgpio;
        int vsyncgpio;
        int datagpios[16];
        uint32_t dbfb;

    void Mode(uint32_t cpixelclockhz, uint32_t hres, uint32_t vres, uint32_t hsyncpw, uint32_t hsyncbw, uint32_t hsyncfw, uint32_t vsyncpw, uint32_t vsyncbw, uint32_t vsyncfw, 
        uint32_t hsyncidlelow, uint32_t vsyncidlelow, uint32_t pclkactivenegative, size_t datawidth, size_t bitsperpixel, size_t num_frbfs, size_t bouncebfrsize,
        size_t sramtransalign, size_t psramtransalign, int hsyncgpio, int vsyncgpio, int datagpios[16], uint32_t dbfb) {

        }
};

#endif