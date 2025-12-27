#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_mac.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "VGARes.h"

VGARes VGARes::R640x480at60hz16bit(25000000, 640, 480, 96, 48, 16, 2, 33, 10, 0, 0, 1, 16, 16, 2, 640*10, 8, 16, 1, 2, {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}, 1);
VGARes VGARes::R640x480at60hz8bit(25000000, 640, 480, 96, 48, 16, 2, 33, 10, 0, 0, 1, 8, 8, 2, 640*10, 8, 16, 1, 2, {4,5,6,7,8,9,10,11,-1,-1,-1,-1,-1,-1,-1,-1}, 1);
VGARes VGARes::R640x480at60hz4bit(25000000, 640, 480, 96, 48, 16, 2, 33, 10, 0, 0, 1, 8, 8, 2, 640*10, 8, 16, 1, 2, {4,5,6,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, 1);
VGARes VGARes::R640x480at60hz3bit(25000000, 640, 480, 96, 48, 16, 2, 33, 10, 0, 0, 1, 8, 8, 2, 640*10, 8, 16, 1, 2, {4,5,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, 1);
